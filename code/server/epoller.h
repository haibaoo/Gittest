#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <errno.h>

class Epoller{
public:
    explicit Epoller(int maxEvent = 1024);

    ~Epoller();    

    bool ADDFd(int fd,uint32_t events);

    bool ModFd(int fd, uint32_t events);

    bool DelFd(int fd);

    int Wait(int timeoutMs = -1);

    int GetEventFd(size_t i) const; //返回对应位序事件的fd

    uint32_t GetEvents(size_t i) const;//返回对应位序注册的事件，如EPOLL_IN、EPOLL_OUT

private:
    int epollFd_;//epoll实例文件描述符

    std::vector<struct epoll_event> events_;  

};

#endif