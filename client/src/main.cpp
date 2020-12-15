/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
#include <iostream>
#include <config.h>
#include <client.h>
#include <boost/property_tree/ptree.hpp>
#include <watcher.h>
#include <files.h>
#include <file_sipper.h>

int main(int argc, char *argv[]) {
     /**
     * CONFIGURATION PHASE
     * Program reads and writes inside config the option with which the program was run
     */


     RawEndpoint re_test;
     re_test.raw_ip_address = "127.0.0.1";
     re_test.port_num = 3343;
     try {
         FileSipper(re_test, "Prova/numbers.txt");
     }catch(std::exception& e){
         std::cerr << "Error " << e.what() << std::endl;
     }

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

    //Check if the backup folder inside config.JSON exists
    if( !std::filesystem::exists(Config::get_Instance()->ReadProperty("path")) ){
        std::cerr << "Backup Folder doesn't exist. Check if you write it correctly and run the program again." << std::endl;
        return 1;
    }
    RawEndpoint raw_endpoint = Config::get_Instance()->ReadRawEndpoint();
    /**
     * The main builds the client that will contain the main logic of the app.
     */
    // Building the Client
    // TODO These could also be a thread?
    std::filesystem::path path = std::filesystem::path(Config::get_Instance()->ReadProperty("path"));
    Client client{raw_endpoint, path};

    //TODO Change this leaving the main in the background.
    std::cin.ignore();
}

