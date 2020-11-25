#include <iostream>
#include <boost/asio.hpp>
#include <server.h>
#include <SQLiteCpp/Database.h>

#include "sqlite3.h"
int main(){

    unsigned short port_num = 3333;
    std::cout << sqlite3_libversion() << std::endl;
    // Open a database file
    //SQLite::Database db("example.db");

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

