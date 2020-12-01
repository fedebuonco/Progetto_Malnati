/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
/* 00 system includes */
#include <iostream>
#include <string>
/* 01 project includes */
#include "config.h"
#include "../includes/client/client.h"
#include <boost/property_tree/ptree.hpp>
#include <tree_t.h>
#include <watcher.h>

//This is the callback the watcher wil call.
int SyncClient(Client client);

int main(int argc, char *argv[]) {

     /**
     * CONFIGURATION PHASE
     * Program reads and writes inside config the option with which the program was run
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
     * VALIDATION PHASE
     * Program checks the validity of the config options
     */

    //TODO Ho notato che le stampe non sono in ordine. Non penso sia un problema
    //Check if the backup folder inside config.JSON exists
    if( !std::filesystem::exists(Config::get_Instance()->ReadProperty("path")) ){
        std::cerr << "Backup Folder doesn't exist. Check if you write it correctly and run the program again." << std::endl;
        return 1;
    }
    RawEndpoint raw_endpoint = Config::get_Instance()->ReadRawEndpoint();

    /**
     * Connect to server and Auth PHASE
     */

    // Creating the Client
    std::filesystem::path path = std::filesystem::path(Config::get_Instance()->ReadProperty("path"));
    Client client{raw_endpoint, path};

    //TODO Change this leaving the client in the background.
    std::cin.ignore();
    // Here the main has everything it needs in order to asyncronously send the patch
    //client.SendPatch(patch);
}

