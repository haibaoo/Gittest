#ifndef BUFFER_H
#define BUFFER_H
#include <cstring>  //bzero()
#include <iostream>
#include <vector>
#include <sys/uio.h>
#include <atomic>
#include <unistd.h>
#include <assert.h>

/**
 * 这个类应该是暂存呈待记录的信息,存储实际用std::vector<char>
*/
class Buffer{
public:
    Buffer(int initBuffSize = 1024);//有参数的都不叫默认构造
    ~Buffer() = default;//默认析构函数

    //常量成员函数，对成员仅读
    size_t WritableBytes() const;       //可写的大小
    size_t ReadableBytes() const ;      //可读的大小
    size_t PrependableBytes() const;    //相当于是可以重分配的字节、这部分的字节数已经读过了

    const char* Peek() const;           //现在可读的位置
    void EnsureWriteable(size_t len);   //确保可写的字节数>=len
    void HasWritten(size_t len);        //更新以写位序

    void Retrieve(size_t len);          //将已读位置更新，即+len
    void RetrieveUntil(const char* end);//将已读位置更新到end

    void RetrieveAll() ;                //将buffer清零
    std::string RetrieveAllToStr();     //将buffer清零，并返回可读的string

    const char* BeginWriteConst() const;//可写的位置const版
    char* BeginWrite();                 //可写的位置

    void Append(const std::string& str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len); //真正的append操作
    void Append(const Buffer& buff);            

    ssize_t ReadFd(int fd, int* Errno);
    ssize_t WriteFd(int fd, int* Errno);

private:
    char *BeginPtr_();                   //返回首元素地址
    const char* BeginPtr_() const;       //const版本
    void MakeSpace_(size_t len);         //保证空间大于len

    std::vector<char> buffer_;  //实际存放的vector
    std::atomic<std::size_t> readPos_;  //已读位置
    std::atomic<std::size_t> writePos_; //已写位置
};

#endif