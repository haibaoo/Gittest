#include "log.h"

Log::Log(){
    lineCount_ = 0;
    isAsync_ = false;
    writeThread_ = nullptr;
    deque_ = nullptr;
    toDay_ = 0;
    fp_ = nullptr;    
}


Log* Log::Instance(){
    static Log inst;
    return &inst;
}

int Log::GetLevel(){
    std::lock_guard<std::mutex> locker(mtx_);
    return level_;
}

/**
 * @brief log初始化
 * @param level 日志等级
 * @param path 日志路径
 * @param suffix 日志后缀
 * @param maxQueueCapacity ？
 * 
*/
void Log::init(int level, const char* path, const char* suffix,
                int maxQueueCapacity){
    isOpen_ = true;
    level_ = level;
    if(maxQueueCapacity > 0){
        isAsync_ = true;//异步
        if(!deque_){
            std::unique_ptr<BlockDeque<std::string>> newDeque(new BlockDeque<std::string>);
            deque_  = std::move(newDeque);

            std::unique_ptr<std::thread> NewThread(new std::thread(FlushLogThread));
            writeThread_ = std::move(NewThread);
        }
    }else{
        isAsync_ = false;
    }

    lineCount_ = 0;

    time_t timer = time(nullptr);
    struct tm *sysTime = localtime(&timer);
    struct tm t = *sysTime;
    path_ = path;
    suffix_ = suffix;
    char fileName[LOG_NAME_LEN] = {0};
    snprintf(fileName, LOG_NAME_LEN - 1,"%s/%04d_%02d_%02d%s",path_,t.tm_year + 1900,t.tm_mon + 1,t.tm_mday,suffix_);
    toDay_ = t.tm_mday;//示例fileName = ./log/2023-07-28.log
    {
        std::lock_guard<std::mutex> locker(mtx_);//C++11特性,可以避免手动调用mtx_.lock(),mtx_unlock()函数
        buff_.RetrieveAll();
        if(fp_){//清除上一次的使用的参数
            flush();
            fclose(fp_);
        }

        fp_ = fopen(fileName,"a");
        if(fp_ == nullptr){
            mkdir(path,0777);
            fp_ = fopen(fileName,"a");
        }
        assert(fp_!=nullptr);
    }
}


/**
 * @brief 向日志文件写内容
*/
void Log::write(int level ,const char* format, ...){

    

}

void Log::flush(){
    if(isAsync_){
        deque_->flush();
    }
}


void Log::AsyncWrite_(){
    std::string str = "";
    while(deque_->pop(str)) {
        std::lock_guard<std::mutex> locker(mtx_);
        fputs(str.c_str(), fp_);
    }    
}

void Log::FlushLogThread(){
    Log::Instance()->AsyncWrite_();
}