#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <stdlib.h>
#include <atomic>
class HttpConn{
public:
    HttpConn();
    ~HttpConn();

    void init(int sockFd, const sockaddr_in &addr);

    void Close();

    int GetFd() const;


static std::atomic<int>  userCount;
static const char* srcDir;
static bool isET;
private:

};
#endif