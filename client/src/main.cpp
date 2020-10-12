/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
/* 00 system includes */
#include <iostream>
/* 01 project includes */
#include "sync_tcp_socket.h"
#include "config.h"
#include "../includes/client/client.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

/* 02 externs */
/* 03 defines */
/* 04 typedefs */
/* 05 globals (but don't)*/
/* 06 ancillary function prototypes if any */

int main(int argc, char *argv[])
{
    RawEndpoint raw_endpoint = Config::get_Instance()->ReadRawEndpoint();

    // Creating the client and auth
    Client client{raw_endpoint};
    client.Auth();
    //From here on, we are authenticated.
    std::cout << "Auth Successfully" << std::endl;
    // Now we will ask for tree computing
    // TODO make this requesttree return the type we want/like/need
    std::cout << "Asking for Tree" << std::endl;
    client.RequestTree();

}
