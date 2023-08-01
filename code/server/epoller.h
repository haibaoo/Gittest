#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <assert.h>
#include <vector>

class Epoller{
public:
    explicit Epoller(int maxEvent = 1024);

    ~Epoller();    

    bool ADDFd(int fd,uint32_t events);

private:
    int epollFd_;

    std::vector<struct epoll_event> events_;  

};

#endif