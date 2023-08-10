#include "buffer.h"

//创建了大小为initBuffSize的std::vector<char>对象
Buffer::Buffer(int initBuffSize) : buffer_(initBuffSize), readPos_(0), writePos_(0){}

size_t Buffer::WritableBytes() const{
    return buffer_.size() -writePos_;
}

size_t Buffer::ReadableBytes() const{
    return writePos_ - readPos_;
}

size_t Buffer::PrependableBytes() const{
    return readPos_;
}

const char*Buffer::Peek() const{

}










//重置Buffer
void Buffer::RetrieveAll(){
    bzero(&buffer_[0],sizeof(buffer_));
    readPos_ = 0;
    writePos_ = 0;
}


