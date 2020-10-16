/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
/* 00 system includes */
#include <iostream>
/* 01 project includes */
#include "config.h"
#include "../includes/client/client.h"
#include <boost/property_tree/ptree.hpp>

int main(int argc, char *argv[])
{
    RawEndpoint raw_endpoint = Config::get_Instance()->ReadRawEndpoint();
    // Creating the client and auth
    Client client{raw_endpoint};
    client.Auth();

    // From here on, we are authenticated.
    // Here we do the first tree diff and then start the monitor where we
    // do the procedure again if we find a change in the
    // choosen directory

    //First Diff
    std::cout << "Asking for Tree for the first time" << std::endl;
    client.RequestTree();
}
