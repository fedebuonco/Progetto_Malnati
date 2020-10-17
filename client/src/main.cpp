/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
/* 00 system includes */
#include <iostream>
#include <utilities.h>
/* 01 project includes */
#include "config.h"
#include "../includes/client/client.h"



int main(int argc, char *argv[])
{
    std::cout << "Numero di argomenti: " << argc << " " << argv[0]<< std::endl;
    //Cient

    //-c fede fede
    //-s 127.0.0.1 3333
    //-f path
    //-v
    //--help print all command
    //--debug


    if(argc>1){     //User starts the program with <options>

        for(int i=1; i<argc; i++){  //Start from 1 because argv[0] is the program name

            std::string arg = argv[i];

            //We need to use 'str2int' function because switch cannot take string but only integer
            switch (str2int(arg.c_str())) {

                case str2int("-c"):
                case str2int("--credential"):



                    break;
                case str2int("-f"):
                case str2int("--folder"):

                    break;
                case str2int("-s"):
                case str2int("--server"):
                    if(DEBUG) std::cout << "Server option detected" << std::endl;

                    break;
                case str2int("-h"):
                case str2int("--help"):
                    show_usage(argv[0]);
                    return 0;

                case str2int("-d"):
                case str2int("--debug"): {
                    DEBUG=true;
                    if(DEBUG) std::cout << "DEBUG activated" <<std::endl;
                    break;
                }

                default:
                    std::cerr << "\nError: The \"" << arg  <<"\" option doesn't exist. Write -h or --help to see the available options"<< std::endl;
            }


        }

    }


/*
    RawEndpoint raw_endpoint = Config::get_Instance()->ReadRawEndpoint();
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
*/
}

