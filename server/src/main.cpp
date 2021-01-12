#include <iostream>
#include <boost/asio.hpp>
#include <server.h>
#include <SQLiteCpp/Database.h>
#include "sqlite3.h"
#include <async_server.h>
#include <filesystem>



std::atomic<bool> is_terminated = false;

void closeServer(int sig){ // can be called asynchronously
    is_terminated = true;
}

const unsigned int DEFAULT_THREAD_POOL_SIZE = 1;

int main(int argc, char *argv[]){

    unsigned short aport_num = 3343;    //Port for the async server
    unsigned short port_num = 3333;     //Port for the sync server

    if(DEBUG) std::cout << sqlite3_libversion() << std::endl;

    //Find the absolute path that points to server folder
    std::filesystem::path mypath = std::filesystem::absolute( std::filesystem::path( argv[0] ) ).remove_filename().parent_path().parent_path();
    if(DEBUG) std::cout<<"PATH: "<<mypath.string()<<std::endl;


    try{
        Server srv(mypath);

        //This asynchronous server manages and stores files sent by clients.
        AsyncServer asrv;

        //Define the number of thread based on the hardware
        unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
        if (thread_pool_size == 0) thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

        //Start the Server and the AsyncServer
        asrv.Start(aport_num, thread_pool_size);
        srv.Start(port_num);

        //The server is running and we wait the termination signal

        // Register signals
        signal(SIGINT, closeServer);
        signal(SIGTERM, closeServer);
        while(true) {
            if(is_terminated) {
                //TODO: Queste due devono essere chiamate anche quando il programma termina senza chiusura utente
                //Inserirle dentro asrv e srv distruttori se non è già stato fatto
                asrv.Stop();
                srv.Stop();
                return 0;
            }
        }
    }catch(...){
            //TODO catch
            std::cerr<<"GENERAL ERROR"<<std::endl;
            std::exit(EXIT_FAILURE);
    }
    return 0;
}

