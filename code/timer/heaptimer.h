#ifndef HEAP_TIMER_H
#define HEAP_TIMER_H

#include <queue>
#include <unordered_map> 
#include <time.h>
#include <algorithm>
#include <arpa/inet.h>
#include <functional>
#include <assert.h>
#include <chrono>
#include "../log/log.h"
                                                //不接受参数
typedef std::function<void()> TimeoutCallBack; //可以用来保存任何可调用对象，函数指针、lambda表达式等
typedef std::chrono::high_resolution_clock Clock;//用于测量时间间隔和时间点
typedef std::chrono::milliseconds MS;//指定时间间隔的类型
typedef Clock::time_point TimeStamp;//用于存放当前时间

struct TimerNode {
    int id;
    TimeStamp expires;              //定义的是expires的时间
    TimeoutCallBack cb;             
    bool operator<(const TimerNode& t) {
        return expires < t.expires;
    }
};

/**
 * 利用时间戳来管理非活跃节点、
*/
class HeapTimer{
public:
    HeapTimer(){ heap_.reserve(64); }//预留的空间为64，但size仍为0。但heap_(64)就是预留的空间和size都等于64，且为默认值

    ~HeapTimer(){ clear(); }

    void adjust(int id, int newExpires);

    void add(int id, int timeOut, const TimeoutCallBack& cb);

    void doWork(int id);

    void clear();

    void tick();

    void pop();

    int GetNextTick();

private:
    void del_(size_t i);
    
    void siftup_(size_t i);

    bool siftdown_(size_t index, size_t n);

    void SwapNode_(size_t i, size_t j);

    std::vector<TimerNode> heap_;

    std::unordered_map<int, size_t> ref_;    
};

#endif