#ifndef LOG_H
#define LOG_H

#include <mutex>
#include <string>
#include <memory>
#include <sys/time.h>
#include <assert.h>
#include <stdarg.h> //可变传参
#include <thread>
#include <sys/stat.h>//mkdir()
#include "blockqueue.h"
#include "../buffer/buffer.h"

class Log{
public:
    void init(int level, const char* path = "./log", 
                const char* suffix =".log",
                int maxQueueCapacity = 1024);
    static Log* Instance();
    static void FlushLogThread();

    void write(int level ,const char* format, ...);
    void flush();

    bool IsOpen(){return isOpen_;}
    int GetLevel();
    void SetLevel(int level);

private:
    Log();
    void AppendLogLevelTitle_(int level);
    virtual ~Log();
    void AsyncWrite_();

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000; //日志行数上限

    const char * path_;
    const char * suffix_;

    int MAX_LINES_;

    int lineCount_;
    int toDay_;

    bool isOpen_;

    Buffer buff_;    
    int level_;
    bool isAsync_;

    FILE* fp_;              //文件指针
    std::unique_ptr<BlockDeque<std::string>> deque_;
    std::unique_ptr<std::thread> writeThread_;
    std::mutex mtx_;

};

//宏定义，跨越多行所以使用"\"。采用单例模式，所以其实都是在使用同一个Log对象在写日志
#define LOG_BASE(level, format, ...) \
    do {\
        Log* log = Log::Instance();\
        if (log->IsOpen() && log->GetLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__); \
            log->flush();\
        }\
    } while(0);

//todo:宏定义中传参的方法、以及可变传参
#define LOG_ERROR(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);

#endif LOG_H