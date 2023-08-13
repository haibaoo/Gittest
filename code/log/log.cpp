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
 * 并将使用痕迹等消除
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

    lineCount_ = 0;//该文件中没写过

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
    struct timeval now = {0, 0}; //
    gettimeofday(&now, nullptr);
    time_t tSec = now.tv_sec;
    struct tm *sysTime = localtime(&tSec);
    struct tm t = *sysTime;
    va_list vaList; //
    
    //超过一天以前写过，或者写过就行
    if(toDay_ != t.tm_mday || (lineCount_ && (lineCount_ % MAX_LINES == 0)))
    {
        std::unique_lock<std::mutex> locker(mtx_);
        locker.unlock();


        char newFile[LOG_NAME_LEN];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);

        if(toDay_ != t.tm_mday)
        {
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s%s", path_, tail, suffix_);
            toDay_ = t.tm_mday;
            lineCount_ = 0;            
        }
        else{
            snprintf(newFile, LOG_NAME_LEN - 72, "%s/%s-%d%s", path_, tail, (lineCount_  / MAX_LINES), suffix_);
        }

        locker.lock();
        flush();
        fclose(fp_);
        fp_ = fopen(newFile,"a");
        assert(fp_ != nullptr);
    }

    {
        std::unique_lock<std::mutex> locker(mtx_); 
        lineCount_++;
        int n = snprintf(buff_.BeginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);//往buffer里写
        
        buff_.HasWritten(n);
        AppendLogLevelTitle_(level);//加日志等级

        va_start(vaList, format);//开始使用可变参数
        int m = vsnprintf(buff_.BeginWrite(), buff_.WritableBytes(), format, vaList);
        va_end(vaList);

        buff_.HasWritten(m);
        buff_.Append("\n\0", 2);

        if(isAsync_ && deque_ && !deque_->full()) {//要是deque没满，就将buffer的内容送到deque
            deque_->push_back(buff_.RetrieveAllToStr());
        } else {
            fputs(buff_.Peek(), fp_);//否则直接输出到文件
        }
        buff_.RetrieveAll();        

    }

}


void Log::AppendLogLevelTitle_(int level){
    switch(level){
    case 0:
        buff_.Append("[debug]: ",9);
        break;
    case 1:
        buff_.Append("[info] : ", 9);
        break;
    case 2:
        buff_.Append("[warn] : ", 9);
        break;
    case 3:
        buff_.Append("[error]: ", 9);
        break;
    default:
        buff_.Append("[info] : ", 9);
        break;            
    }
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