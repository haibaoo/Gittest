#include <unistd.h>
#include <iostream>
int main(int argc,char **argv){
    const char *optstring = "a:b:c:";
    int opt;
    while ((opt = getopt(argc,argv,optstring))!=-1)
    {
        switch (opt)
        {
        case 'a':
            std::cout<<"opt a,optarg is"<<optarg<<std::endl;
            break;
        case 'b':
            std::cout<<"opt b,optarg is"<<optarg<<std::endl;
            break;
        case 'c':
            std::cout<<"opt c,optarg is"<<optarg<<std::endl;
            break;
        default:
            break;
        }
    }
    
}