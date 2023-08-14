#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

class ThreadPool{
public:
    explicit ThreadPool(size_t threadCount = 8) : pool_(std::make_shared<Pool>()) {
            assert(threadCount > 0);//线程数
            for(size_t i = 0;i < threadCount;i++){
                std::thread([pool = pool_] {            //用lambda表达式创建线程执行函数
                    std::unique_lock<std::mutex> locker(pool->mtx);
                    while(true) {
                        if(!pool->tasks.empty()) { 
                            auto task = std::move(pool->tasks.front());
                            pool->tasks.pop();
                            locker.unlock();
                            task();         //任务执行
                            locker.lock();
                        } 
                        else if(pool->isClosed) break;
                        else pool->cond.wait(locker);//等待
                    }
                }).detach();//主线程与子线程分离
            }
    }

    ~ThreadPool(){
        if(static_cast<bool>(pool_)) {
            {
                std::lock_guard<std::mutex> locker(pool_->mtx);
                pool_->isClosed = true;
            }
            pool_->cond.notify_all();
        }        
    }

    ThreadPool() = default;

    ThreadPool(ThreadPool&&) = default;

    template <class F>
    void AddTask(F && task) {
        {
            std::lock_guard<std::mutex> locker(pool_->mtx);
            pool_->tasks.emplace(std::forward<F>(task));//完美转发
        }
        pool_->cond.notify_one();
    }

private:
    struct Pool {
        std::mutex mtx;
        std::condition_variable cond;
        bool isClosed;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_;
};

# endif