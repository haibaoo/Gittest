#include "log.h"

Log* Log::Instance(){
    static Log inst;
    return &inst;
}

int Log::GetLevel(){
    std::lock_guard<std::mutex> locker(mtx_);
    return level_;
}