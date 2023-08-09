#include "buffer.h"

void Buffer::RetrieveAll(){
    bzero(&buffer_[0],sizeof(buffer_));
    readPos_ = 0;
    writePos_ = 0;
}