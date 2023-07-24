#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <unistd.h>
#include <cstdlib>
#include <string>

class Webserver{
public:
    Webserver();
    ~Webserver(){};

    void init(int port,std::string user,std::string passWord,std::string databaseName,
    int log_write,int opt_linger,int trigmode,int sql_num,
    int thread_num,int close_log,int actor_model);

};
#endif