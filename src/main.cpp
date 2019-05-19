#include "Server.hpp"
int main()
{
    Server server;
    if (server.Init() == INIT_SUCCESS){
        server.Start();
    }else {
        std::cout<<"Init failed"<<std::endl;
    }
    return EXIT_SUCCESS;
}