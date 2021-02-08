#include <iostream>
#include <boost/asio.hpp>
#include <server.h>
#include <SQLiteCpp/Database.h>
#include <sqlite3.h>
#include <async_server.h>
#include <filesystem>
#include <accept_client.h>

std::atomic<bool> is_terminated = false;

const unsigned int DEFAULT_THREAD_POOL_SIZE = 1;

int main(int argc, char *argv[]){

    unsigned short port_num = 3333;          //Port for the sync server
    unsigned short async_port_num = 3343;    //Port for the async server

    //Find the absolute path that points to server folder
    std::filesystem::path server_path = std::filesystem::absolute(std::filesystem::path( argv[0] ) ).remove_filename().parent_path().parent_path();
    if(DEBUG) std::cout << "Server path: " << server_path.string() << std::endl;


    try{
        //Synchronous server that handle the request from clients
        Server srv(server_path);

        //This asynchronous server manages and stores files sent by clients.
        AsyncServer asrv(server_path);

        //Define the number of thread based on the hardware
        unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
        if (thread_pool_size == 0) thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

        //Start the Server and the AsyncServer
        asrv.Start(async_port_num, thread_pool_size);
        srv.Start(port_num);

        //The server is running and we wait the termination signal

        // Register termination signals
        signal(SIGINT, [](int sig){ is_terminated.store(true);});
        signal(SIGTERM, [](int sig){ is_terminated.store(true);});
        while(true) {
            if(is_terminated) {

                asrv.Stop();
                srv.Stop();
                std::cout<<"Program successfully closed"<<std::endl;
                break;
            }
        }

    }
    catch(std::exception &e){

        std::cerr<<"Error: " << e.what() <<std::endl;
        std::exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

