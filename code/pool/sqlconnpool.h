#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

class SqlConnPool{
public:
    static SqlConnPool *Instance();


private:
    SqlConnPool();
    ~SqlConnPool();

};
#endif