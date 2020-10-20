#include <iostream>
#include <boost/asio.hpp>
#include <server.h>

int main(){

    unsigned short port_num = 3333;

    try{
        Server srv;
        srv.Start(port_num);

        std::this_thread::sleep_for(std::chrono::seconds(600));

        srv.Stop();
    }catch(...){
        //TODO catch
    }
    return 0;
}

