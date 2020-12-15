#include <iostream>
#include <boost/asio.hpp>
#include <server.h>
#include <SQLiteCpp/Database.h>
#include <async_server.h>

#include "sqlite3.h"

const unsigned int DEFAULT_THREAD_POOL_SIZE = 1;

int main(){
    unsigned short aport_num = 3343;
    unsigned short port_num = 3333;
    std::cout << sqlite3_libversion() << std::endl;

    try{
        Server srv;
        AsyncServer asrv;

        unsigned int thread_pool_size =
                std::thread::hardware_concurrency() * 2;

        if (thread_pool_size == 0)
            thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

        asrv.Start(aport_num, thread_pool_size);
        srv.Start(port_num);

        std::this_thread::sleep_for(std::chrono::seconds(600));

        asrv.Stop();
        srv.Stop();
    }catch(...){
        //TODO catch
    }
    return 0;
}

