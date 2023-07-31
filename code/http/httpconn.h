#ifndef HTTP_CONN_H
#define HTTP_CONN_H
#include <atomic>
class HttpConn{
public:
    HttpConn();
    ~HttpConn();


static std::atomic<int> userCount;
static const char* srcDir;
static bool isET;
private:

};
#endif