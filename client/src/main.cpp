/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
/* 00 system includes */
#include <iostream>
#include <string>
/* 01 project includes */
#include "config.h"
#include <utilities.h>
#include "../includes/client/client.h"



int main(int argc, char *argv[]) {

    /**
     * CONFIGURATION PHASE
     */

    //User starts the program with <options>
    if (argc > 1) {
        int value = Config::get_Instance()->SetConfig(argc, argv);

        //TODO Per me è meglio usare eccezione
        if(value==1){
            return 1;
        }

    }

    Config::get_Instance()->PrintConfiguration();


    /**
     * VALIDATION AND STARTUP PHASE
     */


    RawEndpoint raw_endpoint = Config::get_Instance()->ReadRawEndpoint();




    //TODO Vedere le funzioni sotto e togliere i file divisi
    // Creating the client and auth

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
