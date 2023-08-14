#include "webserver.h"

/**
 * @brief 初始化server
 * 
*/
Webserver::Webserver(int port, int trigMode, int timeoutMS, bool OptLinger, 
        int sqlPort, const char* sqlUser, const  char* sqlPwd, 
        const char* dbName, int connPoolNum, int threadNum,
        bool openLog, int logLevel, int logQueSize):port_(port), openLinger_(OptLinger), timeoutMS_(timeoutMS), isClose_(false),
            timer_(new HeapTimer()), threadpool_(new ThreadPool(threadNum)), epoller_(new Epoller())
    {
    srcDir_ = getcwd(nullptr,256);
    assert(srcDir_);
    strcat(srcDir_,"/resources/");//char 字符串用这个，string可以直接用+
    
    HttpConn::userCount = 0;//todo
    HttpConn::srcDir = srcDir_;
    SqlConnPool::Instance()->Init("localhost",sqlPort,sqlUser,sqlPwd,dbName,connPoolNum);//初始化数据库连接池

    InitEventMode_(trigMode);//设置epoll触发模式

    if(!InitSocket_()){ isClose_ = true;}

    if(openLog){
        Log::Instance()->init(logLevel, "./log", ".log", logQueSize);//
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

void Webserver::InitEventMode_(int trigMode){
    listenEvent_ = EPOLLRDHUP;  //表示被监听的描述符出现被挂断或者错误时触发
    connEvent_ = EPOLLONESHOT | EPOLLRDHUP;//ONESHOT表示只监听一次该事件，监听完这次事件后，如果还要继续监听则再次加入EPOLL队列
    switch(trigMode)
    {
    case 0:
        break;
    case 1:
        connEvent_ |=EPOLLET;
        break;
    case 2:
        listenEvent_ |= EPOLLET;
        break;
    case 3:
        listenEvent_ |= EPOLLET;
        connEvent_ |= EPOLLET;
        break;
    default:
        listenEvent_ |= EPOLLET;
        connEvent_ |= EPOLLET;
        break; 
    }
    HttpConn::isET = (connEvent_ & EPOLLET);
}

/**
 * @brief 
*/
void Webserver::Start(){
    int timeMS = -1;
    if(!isClose_) { LOG_INFO("========== Server start =========="); }//socket失败
    while(!isClose_){
        if(timeoutMS_ > 0){
            timeMS = timer_->GetNextTick();
        }

        int eventCnt = epoller_->Wait(timeMS);//
        for(int i = 0; i < eventCnt; i++) {
            /* 处理事件 */
            int fd = epoller_->GetEventFd(i);
            uint32_t events = epoller_->GetEvents(i);
            if(fd == listenFd_) {
                DealListen_();
            }
            else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {//处理异常
                assert(users_.count(fd) > 0);
                CloseConn_(&users_[fd]);
            }
            else if(events & EPOLLIN) {//处理读事件
                assert(users_.count(fd) > 0);//count检查fd是否在map中
                DealRead_(&users_[fd]);
            }
            else if(events & EPOLLOUT) {//处理写事件
                assert(users_.count(fd) > 0);
                DealWrite_(&users_[fd]);
            } else {
                LOG_ERROR("Unexpected event");
            }
        }        

    }

}

void Webserver::SendError_(int fd, const char*info){        
    assert(fd > 0);
    int ret = send(fd,info,strlen(info),0);
    if(ret < 0){
        LOG_WARN("send error to client[%d] error!", fd);
    }
    close(fd);
}

void Webserver::CloseConn_(HttpConn* client) {
    assert(client);
    LOG_INFO("Client[%d] quit!", client->GetFd());
    epoller_->DelFd(client->GetFd());
    client->Close();
}


void Webserver::AddClient_(int fd, sockaddr_in addr) {
    assert(fd > 0);
    users_[fd].init(fd, addr);//
    if(timeoutMS_ > 0) {//超时时间都设为60s
        timer_->add(fd, timeoutMS_, std::bind(&Webserver::CloseConn_, this, &users_[fd]));
    }
    epoller_->ADDFd(fd, EPOLLIN | connEvent_);
    SetFdNonblock(fd);
    LOG_INFO("Client[%d] in!", users_[fd].GetFd());
}

void Webserver::DealListen_() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    do {
        int fd = accept(listenFd_, (struct sockaddr *)&addr, &len);//非阻塞，因为listenFd设置为非阻塞
        if(fd <= 0) { return;}//没有接收到连接请求
        else if(HttpConn::userCount >= MAX_FD) {
            SendError_(fd, "Server busy!");
            LOG_WARN("Clients is full!");
            return;
        }
        AddClient_(fd, addr);
    } while(listenEvent_ & EPOLLET);
}

void Webserver::DealRead_(HttpConn* client) {
    assert(client);
    ExtentTime_(client);
    threadpool_->AddTask(std::bind(&Webserver::OnRead_, this, client));
}

void Webserver::DealWrite_(HttpConn* client) {
    assert(client);
    ExtentTime_(client);
    threadpool_->AddTask(std::bind(&Webserver::OnWrite_, this, client));
}


void Webserver::ExtentTime_(HttpConn* client) {
    assert(client);
    if(timeoutMS_ > 0) { timer_->adjust(client->GetFd(), timeoutMS_); }
}

bool Webserver::InitSocket_(){
    int ret;
    struct sockaddr_in addr;
    if(port_<1124 || port_>65535){
        LOG_ERROR("Port%d error!",port_);
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    struct linger optlinger = {0};
    if(openLinger_){//优雅关闭的标志位
        optlinger.l_onoff = 1;//打开
        optlinger.l_linger = 1;//等待时间
    }
    listenFd_ = socket(AF_INET,SOCK_STREAM,0);
    if(listenFd_ < 0) {
        LOG_ERROR("Create socket error!", port_);
        return false;
    }

    //设置套接字的SO_LINGER选项
    ret = setsockopt(listenFd_,SOL_SOCKET,SO_LINGER,&optlinger,sizeof(optlinger));
    if(ret == -1) {
        close(listenFd_);
        LOG_ERROR("Init linger error!", port_);
        return false;
    }

    ret = bind(listenFd_,(const sockaddr *)&addr,sizeof(addr));
    if(ret < 0) {
        LOG_ERROR("Bind Port:%d error!", port_);
        close(listenFd_);
        return false;
    }    

    ret = listen(listenFd_, 6);
    if(ret < 0) {
        LOG_ERROR("Listen port:%d error!", port_);
        close(listenFd_);
        return false;
    }

    ret = epoller_->ADDFd(listenFd_, listenEvent_ | EPOLLIN);
    if(ret == 0) {
        LOG_ERROR("Add listen error!");
        close(listenFd_);
        return false;
    }
    SetFdNonblock(listenFd_);//accept的时候不阻塞
    LOG_INFO("Server port:%d", port_);
    return true;
}

int Webserver::SetFdNonblock(int fd)
{
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}

