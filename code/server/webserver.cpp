#include "webserver.h"

Webserver::Webserver(int port, int trigMode, int timeoutMS, bool OptLinger, 
        int sqlPort, const char* sqlUser, const  char* sqlPwd, 
        const char* dbName, int connPoolNum, int threadNum,
        bool openLog, int logLevel, int logQueSize):port_(port), openLinger_(OptLinger), timeoutMS_(timeoutMS), isClose_(false),
            timer_(new HeapTimer()), threadpool_(new ThreadPool(threadNum)), epoller_(new Epoller())
    {
    srcDir_ = getcwd(nullptr,256);
    assert(srcDir_);
    strcat(srcDir_,"/resources/");
    
    HttpConn::userCount = 0;//todo
    HttpConn::srcDir = srcDir_;
    SqlConnPool::Instance()->Init("localhost",sqlPort,sqlUser,sqlPwd,dbName,connPoolNum);//初始化数据库连接池

    InitEventMode_(trigMode);//设置epoll触发模式

    if(!InitSocket_()){ isClose_ = true;}

    if(openLog){
        Log::Instance()->init(logLevel, "./log", ".log", logQueSize);
        if(isClose_){LOG_ERROR("========== Server init error!==========");}
        else{
            LOG_INFO("========== Server init ==========");
            LOG_INFO("Port:%d, OpenLinger: %s", port_, OptLinger? "true":"false");
            LOG_INFO("Listen Mode: %s, OpenConn Mode: %s",
                            (listenEvent_ & EPOLLET ? "ET": "LT"),
                            (connEvent_ & EPOLLET ? "ET": "LT"));
            LOG_INFO("LogSys level: %d", logLevel);
            LOG_INFO("srcDir: %s", HttpConn::srcDir);
            LOG_INFO("SqlConnPool num: %d, ThreadPool num: %d", connPoolNum, threadNum);
        }
    }
}



bool Webserver::InitSocket_(){


}

void Webserver::InitEventMode_(int trigMode){
    


}
