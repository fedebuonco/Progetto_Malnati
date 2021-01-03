
#include <iostream>
#include <boost/asio.hpp>
#include <server.h>
#include <SQLiteCpp/Database.h>
#include "sqlite3.h"

#include <filesystem>

volatile sig_atomic_t flag = 0;
void closeServer(int sig){ // can be called asynchronously
    flag = 1; // set flag
}


int main(int argc, char *argv[]){

    unsigned short port_num = 3333;
    std::cout << sqlite3_libversion() << std::endl;
    std::filesystem::path mypath = std::filesystem::absolute( std::filesystem::path( argv[0] ) ).remove_filename().parent_path().parent_path();
    std::cout<<"PATH: "<<mypath.string()<<std::endl;


    try{
        Server srv(mypath);
        srv.Start(port_num);

        // Register signals
        signal(SIGINT, closeServer);
        while(1)
            if(flag){ // my action when signal set it 1
                std::cout<<"Shutdown Server"<<std::endl;
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

