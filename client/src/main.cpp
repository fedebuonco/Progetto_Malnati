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
    // We need to generate our folder structure
    std::cout << "Generating my Folder tree" << std::endl;

    std::string client_tree;
    std::string server_tree;

    client_tree = client.GenerateTree(std::filesystem::path("Prova"));

    //then we ask for the server's
    std::cout << "Asking for Tree for the first time" << std::endl;
    server_tree = client.RequestTree();
    //std::cout << server_tree << std::endl;

    //And we can compute the Diff
    std::string diff = client.GenerateDiff(client_tree,server_tree);
    std::cout << diff << std::endl;
}
