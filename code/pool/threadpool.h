#ifndef THREADPOOL_H
#define THREADPOOL_H

class ThreadPool{
public:
    ThreadPool(size_t threadCount = 8);
    ~ThreadPool();
};

#endif