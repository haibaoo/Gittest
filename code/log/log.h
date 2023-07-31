#ifndef LOG_H
#define LOG_H
#include <mutex>

class Log{
public:
    void init(int level, const char* path = "./log", 
                const char* suffix =".log",
                int maxQueueCapacity = 1024);
    static Log* Instance();
    bool IsOpen(){return isOpen_;}
    int GetLevel();

private:
    Log();
    ~Log();
    bool isOpen_;
    int level_;


    std::mutex mtx_;

};

#define LOG_BASE(level,format,...) \ 
    do{\
    Log *log = Log::Instance();\
    if(log->IsOpen() && log->GetLevel()<=level){\
    }\
}\
while(0);

//todo:宏定义中传参的方法、以及可变传参
#define LOG_ERROR(format,...)do{LOG_BASE(0,format,##__VA_ARGS__)}while(0);
#define LOG_INFO(format,...)do{LOG_BASE(1,format,##_VA_ARGS__)}while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);

#endif LOG_H