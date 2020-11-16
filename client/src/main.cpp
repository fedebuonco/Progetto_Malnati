/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
/* 00 system includes */
#include <iostream>
#include <string>
/* 01 project includes */
#include "config.h"
#include <utilities.h>
#include "../includes/client/client.h"
#include <boost/property_tree/ptree.hpp>
#include <tree_t.h>



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
     * CONNECT TO ENDPOINT AND AUTHENTICATION PHASE
     */

    // Creating the Client and Auth
    Client client{raw_endpoint};

    //Return true or false
    bool authentication_status = client.Auth();

    //Check if the user is authenticated or not
    if(!authentication_status){
        std::cerr << "Username and/or password are not correct" << std::endl;
        return 1;
    }

    // From here on, we are authenticated.

    /**
     *  Monitor PHASE
     *  Here we and start the monitor where
     */

    // we loop detecting a change in the dir.

    //Generate Client tree string
    std::string client_tree;
    std::filesystem::path monitored_folder = Config::get_Instance()->ReadProperty("path");
    client_tree = client.GenerateTree(monitored_folder);

    // Then we ask for the Server's TreeT ( Tree string and times )
    TreeT server_th = client.RequestTree();

    //And we can compute the Diff and store it in a Patch
    Patch update = client.GeneratePatch(client_tree, server_th.tree_);

    // Now we process the patch, preparing all the needed data structures
     client.ProcessRemoved(update);
     client.ProcessCommon(update,server_th);
     client.ProcessNew(update);

    if (DEBUG)
        update.PrettyPrint();

    // Here the main has everything it needs in order to asyncronously send the patch
    //client.SendPatch(patch);
    
}
