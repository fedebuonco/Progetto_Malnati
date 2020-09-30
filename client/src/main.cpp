/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/

/* 00 system includes */

#include <iostream>

/* 01 project includes */

#include "sync_tcp_socket.h"
#include "config.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


/* 02 externs */
/* 03 defines */
/* 04 typedefs */
/* 05 globals (but don't)*/
/* 06 ancillary function prototypes if any */

int main(int argc, char *argv[])
{
    RawEndpoint connection_ =  Config::get_Instance()->ReadConnection();

    //TODO valutare se mettere try catch
    //sync socket for auth creation
    SyncTCPSocket client_sync(connection_.raw_ip_address, connection_.port_num);
    //socket connecting, will be used for authentication
    client_sync.ConnectServer(5);

    if(! Config::get_Instance()->isConfig() ){
        Config::get_Instance()->startConfig();
    }

    //TODO Lanciare detro isConfig e startConfig delle eccezioni di una classe
    // da fare e poi catcharle nel main per terminare il programma.
    // Es. No indirizzo ip per connessione server

    //Auth loop, while the password is wrong asks for a new identity. Will exit as soon as
    //data inserted is verified correctly.
    while( !client_sync.Authenticate() ){
        Config::get_Instance()->startConfig();
    }

    std::cout << "AUTENTICATOOOO" << std::endl;

/* 07 variable declarations */
/* 08 check argv[0] to see how the program was invoked */
/* 09 process the command line options from the user */
/* 10 do the needful */
}
