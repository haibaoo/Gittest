#include "config.h"

int main(int argc,char**argv){
    //给定数据库、账号、密码
    std::string user = "root";
    std::string passwd = "uestc8020";
    std::string databasename = "gzjtestdb";

    //解析命令行、修改参数
    Config config;
    config.parsearg(argc,argv);

    

}