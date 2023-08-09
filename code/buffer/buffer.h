#ifndef BUFFER_H
#define BUFFER_H
#include <cstring>  //bzero()
#include <iostream>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <assert.h>
/**
 * 这个类应该是用来记录日志的相关信息，应该是暂存待记录的信息
*/
class Buffer{
public:
    Buffer(int initBuffSize = 1024);
    ~Buffer() = default;//默认析构函数

    void RetrieveAll();

private:

std::vector<char> buffer_;
std::atomic<std::size_t> readPos_;
std::atomic<std::size_t> writePos_;
};
#endif