/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
#include <iostream>
#include <config.h>
#include <client.h>
#include <filesystem>
#include <queue>
#include <sender.h>

std::atomic<bool> is_terminated = false;

int main(int argc, char *argv[]) {

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
    Client client{raw_endpoint, std::filesystem::path(Config::get_Instance()->ReadProperty("path")) };


    Sender sender;
    std::thread thread_sender( [&sender]() {
        sender.Sender_Action();
    });


    //Install a signal handler. Need a sig parameter.
    signal(SIGINT, [](int sig){ is_terminated.store(true);});
    signal(SIGTERM, [](int sig){ is_terminated.store(true);});

    while(true) {
        if(is_terminated) {

            sender.setFlag(false);
            SharedQueue::get_Instance()->setFlag(false); //SharedQueue is block inside a CV. We release and terminate them

            thread_sender.join();
            std::cout<<"Program successfully closed"<<std::endl;
            break;
        }
    }

    return EXIT_SUCCESS;
}

