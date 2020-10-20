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

    // Creating the Client and Auth
    Client client{raw_endpoint};
    client.Auth();

    // From here on, we are authenticated.
    // Here we and start the monitor where
    // we loop detecting a change in the dir.

    std::string client_tree;
    std::string server_tree;

    //Generate Client tree
    client_tree = client.GenerateTree(std::filesystem::path("Prova"));

    // Then we ask for the Server's
    server_tree = client.RequestTree();

    //And we can compute the Diff
    std::string diff = client.GenerateDiff(client_tree,server_tree);
    std::cout << diff << std::endl;
}
