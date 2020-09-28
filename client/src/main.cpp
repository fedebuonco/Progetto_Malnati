/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/

/* 00 system includes */

#include <iostream>

/* 01 project includes */

#include "connect_server.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "config.h"

/* 02 externs */
/* 03 defines */
/* 04 typedefs */
/* 05 globals (but don't)*/
/* 06 ancillary function prototypes if any */

int main(int argc, char *argv[])
{


    if(! Config::get_Instance()->isConfig() ){
        Config::get_Instance()->startConfig();
    }

    // Retrive Server Info
    namespace pt = boost::property_tree;

    //This is the tree root; inside there is the username and password (if the app is config)
    pt::ptree  root;

    // TODO gestire errori nella lettura del json
    //Read the file and put the content inside root
    pt::read_json("../config_file/connection.json", root);

    auto raw_ip_address = root.get<std::string>("ip");
    auto port_num = root.get<unsigned short>("port");

    // RAII, Constructor create connection and destructor exits from it.
    try {
        ConnectServer connector(raw_ip_address, port_num);
    } catch(...){
        std::cout << "ciao" <<std::endl;
    }


/* 07 variable declarations */
/* 08 check argv[0] to see how the program was invoked */
/* 09 process the command line options from the user */
/* 10 do the needful */
}
