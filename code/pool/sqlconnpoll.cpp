#include "sqlconnpool.h"
using namespace std;

//初始化数据库连接池、并初始化信号量
void SqlConnPool::Init(const char* host, int port,
            const char* user,const char* pwd, const char* dbName,
            int connSize = 10) {
    assert(connSize); 
    for(int i=0;i < connSize;i++){
        MYSQL *sql = nullptr;
        sql = mysql_init(sql);
        if(!sql){
            LOG_ERROR("MySql init error!");
            assert(sql);//直接退出
        }
        sql=mysql_real_connect(sql,host,user,pwd,dbName,port,nullptr,0);//链接数据库，成功则返回指向MYSQL结构体的指针
        if(!sql){
            LOG_ERROR("MySql Connect error!");
        }
        connQue_.push(sql);
    }
    MAX_CONN_ = connSize;
    sem_init(&semId_,0,MAX_CONN_);//0表示线程间同步，1表示进程间。semId_表示信号量对象的指针、MAX_CONN_指定信号量初始值
}