
#include <iostream>
#include <boost/asio.hpp>
#include <server.h>
#include <SQLiteCpp/Database.h>
#include <windows.h>
#include "sqlite3.h"

volatile sig_atomic_t flag = 0;
void closeServer(int sig){ // can be called asynchronously
    flag = 1; // set flag
}


int main(){

    unsigned short port_num = 3333;
    std::cout << sqlite3_libversion() << std::endl;



    try{
        Server srv;
        srv.Start(port_num);

        // Register signals
        signal(SIGINT, closeServer);
        while(1)
            if(flag){ // my action when signal set it 1
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

