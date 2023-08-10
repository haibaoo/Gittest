#ifndef BUFFER_H
#define BUFFER_H
#include <cstring>  //bzero()
#include <iostream>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <assert.h>
/**
 * 这个类应该是暂存呈待记录的信息,底层实际用std::vector<char>
*/
class Buffer{
public:
    Buffer(int initBuffSize = 1024);//有参数的都不叫默认构造
    ~Buffer() = default;//默认析构函数


    size_t WritableBytes() const;       //常量成员函数，对成员仅读
    size_t ReadableBytes() const ;
    size_t PrependableBytes() const;

    const char* Peek() const;
    void EnsureWriteable(size_t len);
    void HasWritten(size_t len);

    void Retrieve(size_t len);
    void RetrieveUntil(const char* end);

    void RetrieveAll() ;
    std::string RetrieveAllToStr();

    const char* BeginWriteConst() const;
    char* BeginWrite();

    void Append(const std::string& str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    void Append(const Buffer& buff);

    ssize_t ReadFd(int fd, int* Errno);
    ssize_t WriteFd(int fd, int* Errno);

private:
    char *BeginPtr_();
    const char* BeginPtr_() const;
    void MakeSpace_(size_t len);

    std::vector<char> buffer_;
    std::atomic<std::size_t> readPos_;  //原子操作
    std::atomic<std::size_t> writePos_;
};

#endif