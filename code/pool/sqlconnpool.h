#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H
#include <assert.h>
#include <queue>
#include <mysql/mysql.h>
#include <semaphore.h>
#include "../log/log.h"

class SqlConnPool{
public:
    static SqlConnPool *Instance();

    void Init(const char* host, int port,
              const char* user,const char* pwd, 
              const char* dbName, int connSize);



private:
    SqlConnPool();
    ~SqlConnPool();

    int MAX_CONN_;

    std::queue<MYSQL *> connQue_; 
    std::mutex mtx_;
    sem_t semId_;//控制访问数据库连接池的信号量
};
#endif