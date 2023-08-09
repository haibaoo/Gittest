#ifndef BLOCKQUEUE_H
#define BLOCKQUEUE_H
#include <mutex>
#include <deque>
#include <assert.h>
#include <condition_variable>

/**
 * 模板类的定义方式：
 * 这里相当于是将std::deque封装了一层，封装成为线程安全的双端队列
*/
template <class T>
class BlockDeque{
public:
    explicit BlockDeque(size_t MaxCapacity = 1000);

    ~BlockDeque();

    void clear();

    bool empty();
    
    bool full();

    void Close();

    size_t size();

    size_t capacity();

    T front();

    T back();

    void push_back(const T &item);

    void push_front(const T &item);

    bool pop(T &item);

    bool pop(T &item, int timeout);

    void flush();

private:
    std::deque<T> deq_;

    size_t capacity_;

    std::mutex mtx_;

    bool isClose_;

    std::condition_variable condConsumer_;//条件变量
    
    std::condition_variable condProducer_;

};


template <class T>
BlockDeque<T>::BlockDeque(size_t MaxCapacity):capacity_(Maxcapacity){
    assert(MaxCapacity);
    isClose_ = false;
}

template <class T>
BlockDeque<T>::~BlockDeque(){
    Close();
}

template <class T>
void BlockDeque<T>::Close(){
    {
        std::lock_guard(std::mutex) locker(mtx_);//lock_guard自动申请释放mtx
        deq_.clear();
        isClose_ = true;
    }
    condProducer_.notify_all();
    condConsumer_.notify_all();
}



template <class T>
void BlockDeque<T>::flush(){
    condConsumer_.notify_one();
}
#endif