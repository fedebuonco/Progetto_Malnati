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
