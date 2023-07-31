#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <memory>
#include <unistd.h>
#include <assert.h>
#include <unordered_map>
#include <string.h>
#include "../timer/heaptimer.h"
#include "../pool/threadpool.h"
#include "epoller.h"
#include "../http/httpconn.h"

class Webserver{
public:
    Webserver(int port, int trigMode, int timeoutMS, bool OptLinger, 
        int sqlPort, const char* sqlUser, const  char* sqlPwd, 
        const char* dbName, int connPoolNum, int threadNum,
        bool openLog, int logLevel, int logQueSize);
    ~Webserver();
    void Start();
private:




    int port_;
    bool openLinger_;
    int timeoutMS_;/*毫秒*/
    bool isClose_;
    int listenFd_;
    char *srcDir_;

    std::unique_ptr<HeapTimer> timer_;
    std::unique_ptr<ThreadPool> threadpool_;
    std::unique_ptr<Epoller> epoller_;
    std::unordered_map<int,HttpConn> user_;

};

#endif