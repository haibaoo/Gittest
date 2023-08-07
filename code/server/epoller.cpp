#include "epoller.h"

/**
 * 
 * @param 
 * epollFd_:epoll实例描述符（同时监控上限为512）、
 * @return 
 * */
Epoller::Epoller(int maxEvent):epollFd_(epoll_create(512)), events_(maxEvent){
    assert(epollFd_ >= 0 && events_.size() > 0);
}

bool Epoller::ADDFd(int fd,uint32_t events){
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return 0 == epoll_ctl(epollFd_,EPOLL_CTL_ADD,fd,&ev);
}