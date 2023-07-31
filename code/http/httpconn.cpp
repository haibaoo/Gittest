#include "httpconn.h"
using namespace std;

/*这里为什么声明了这三个变量它就是左值了*/
// std::atomic<int> HttpConn::userCount;
// const char* HttpConn::srcDir;
bool HttpConn::isET;
