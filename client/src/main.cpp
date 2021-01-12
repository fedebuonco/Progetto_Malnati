/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
#include <iostream>
#include <config.h>
#include <client.h>
#include <filesystem>
#include <queue>
#include <sender.h>
#include "file_sipper.h"

volatile sig_atomic_t flag = 0;
void closeServer(int sig){ // can be called asynchronously
    flag = 1; // set flag
}

void Stop(){

}


int main(int argc, char *argv[]) {

    //QUEUE
    Shared_Queue sq;



    /**
    * CONFIGURATION AND VALIDATION PHASE
    * Program reads from the command line the new configuration options and writes it inside config.json
    * Program validates the configuration parameters.
    */
    Config::get_Instance()->SetPath(argv[0]);

     //Before starting the configuration, we check if the config structure (file and folder) are correct
     if(!Config::get_Instance()->IsConfigStructureCorrect()){

         //Structure is not correct, so we restore with the default config structure
         Config::get_Instance()->SetDefaultConfig();
     }


    if (argc > 1) {
        //The user provides some configuration arguments from the command line
        try {
            //It tries to apply these new configuration options to the config_file
            Config::get_Instance()->SetConfig(argc, argv);
        }
        catch (std::exception& e){
            //TODO: For me exit_failure is ok, we don't need an error code
            std::cerr << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    //Shows the configuration with which the program is launched
    Config::get_Instance()->PrintConfiguration();

    //Check if the backup folder inside config.JSON exists
    if( !std::filesystem::exists(Config::get_Instance()->ReadProperty("path")) ){
        std::cerr << "Backup Folder doesn't exist. Check if you write it correctly and run the program again." << std::endl;
        return EXIT_FAILURE;
    }

    RawEndpoint raw_endpoint = Config::get_Instance()->ReadRawEndpoint();

    /**
     * The main builds the client that will contain the main logic of the app.
     */

    // Building the Client
    // TODO These could also be a thread?
    std::filesystem::path path = std::filesystem::path(Config::get_Instance()->ReadProperty("path"));


    std::thread ts( [&sq]() {
        Sender sender(static_cast<std::shared_ptr<Shared_Queue>>(&sq));
        sender.Sender_Action();
    });


    Client client{raw_endpoint, path};

    //TODO Change this leaving the main in the background.
    //TODO We need something to close the client once is running like in the server.
    //std::cin.ignore();



}

