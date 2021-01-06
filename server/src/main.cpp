
#include <iostream>
#include <boost/asio.hpp>
#include <server.h>
#include <SQLiteCpp/Database.h>
#include "sqlite3.h"
#include <async_server.h>
#include <filesystem>

volatile sig_atomic_t flag = 0;
void closeServer(int sig){ // can be called asynchronously
    flag = 1; // set flag
}
const unsigned int DEFAULT_THREAD_POOL_SIZE = 1;

int main(int argc, char *argv[]){

    unsigned short aport_num = 3343;
    unsigned short port_num = 3333;
    std::cout << sqlite3_libversion() << std::endl;
    std::filesystem::path mypath = std::filesystem::absolute( std::filesystem::path( argv[0] ) ).remove_filename().parent_path().parent_path();
    std::cout<<"PATH: "<<mypath.string()<<std::endl;


    try{
        Server srv(mypath);
       
        AsyncServer asrv;

        unsigned int thread_pool_size =
                std::thread::hardware_concurrency() * 2;

        if (thread_pool_size == 0)
            thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

        asrv.Start(aport_num, thread_pool_size);
        srv.Start(port_num);

        // Register signals
        signal(SIGINT, closeServer);
        while(1)
            if(flag){ // my action when signal set it 1
                std::cout<<"Shutdown Server"<<std::endl;
                asrv.Stop();
                srv.Stop();
                break;
            }

    }catch(...){
            //TODO catch
            std::cerr<<"GENERAL ERROR"<<std::endl;
            std::exit(0);

    }
        return 0;
}

