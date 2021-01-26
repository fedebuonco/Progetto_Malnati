/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
#include <iostream>
#include <config.h>
#include <client.h>
#include <filesystem>
#include <queue>
#include <sender.h>

//TODO: da terminmare
std::atomic<bool> is_terminated = false;

void closeServer(int sig){ // can be called asynchronously
    is_terminated = true;
}


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

    //std::thread thread_client( [ = ]() {
        Client client{raw_endpoint, std::filesystem::path(Config::get_Instance()->ReadProperty("path")) };
    //});

    Sender sender;
    std::thread thread_sender( [&sender]() {        //TODO:Controllare &
        sender.Sender_Action();
    });


    //TODO Change this leaving the main in the background.
    //TODO We need something to close the client once is running like in the server.


    signal(SIGINT, closeServer);
    signal(SIGTERM, closeServer);

    while(true) {
        if(is_terminated) {
            //TODO: Queste due devono essere chiamate anche quando il programma termina senza chiusura utente
            //Inserirle dentro asrv e srv distruttori se non è già stato fatto
            sender.setFlag(false);
            SharedQueue::get_Instance()->setFlag(false); //Fermiamo anche shared queue bloccato nella cv
            std::cout<<" WHO IS "<<std::endl;
            //thread_client.join();
            std::cout<<" client "<<std::endl;
            thread_sender.join();
            std::cout<<" sender "<<std::endl;

            return 0;
        }

        //Menu

        //scelta


    }


}

