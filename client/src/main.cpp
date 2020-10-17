/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
/* 00 system includes */
#include <iostream>
#include <string>
/* 01 project includes */
#include "config.h"
#include <utilities.h>
#include "../includes/client/client.h"



int main(int argc, char *argv[]) {

    if (argc > 1) {     //User starts the program with <options>

        for (int i = 1; i < argc; ++i) {  //Start from 1 because argv[0] is the program name

            std::string arg = argv[i];

            //We need to use 'str2int' function because switch cannot take string but only integer
            switch (str2int(arg.c_str())) {

                case str2int("-c"):
                case str2int("--credential"): {

                    //We check first if there are other 2 arguments after "-c" (username and password)
                    //Then we check if this two arguments are not options (prevent the case "-c username -h")
                    if (i + 2 < argc && !(static_cast<std::string>(argv[i + 1]).rfind('-', 0) == 0 ||
                                          static_cast<std::string>(argv[i + 2]).rfind('-', 0) == 0)) {
                        std::string username = argv[++i];
                        std::string password = argv[++i];

                        Config::get_Instance()->writeConfig(username, password);

                    } else {
                        std::cerr
                                << "Syntax error: We not find 2 arguments between the -c or --credential and the next option"
                                << std::endl;
                        return 1;
                    }

                    break;
                }
                case str2int("-f"):
                case str2int("--folder"): {

                    if (i + 1 < argc && static_cast<std::string>(argv[i + 1]).rfind('-', 0) != 0){

                        std::string path = argv[++i];

                        Config::get_Instance()->WriteFolderPath(path);

                    } else {
                        std::cerr
                                << "Syntax error: We not find 1 valid arguments after the -f or --folder"
                                << std::endl;
                        return 1;
                    }

                }
                    break;
                case str2int("-s"):
                case str2int("--server"): {

                    if (i + 2 < argc && !(static_cast<std::string>(argv[i + 1]).rfind('-', 0) == 0 ||
                                          static_cast<std::string>(argv[i + 2]).rfind('-', 0) == 0)) {

                        std::string ip = argv[++i];
                        std::string port = argv[++i];

                        //TODO CHECK NEL METODO PER VERIFICARE CORRETTEZZA IP E PORT E POI VALUTARE SE LANCIARE ECCEZIONE
                        Config::get_Instance()->WriteRawEndpoint(ip, port);

                    } else {
                        std::cerr
                                << "Syntax error: We not find 2 valid arguments after the -s or --server"
                                << std::endl;
                        return 1;
                    }

                    break;

                }
                case str2int("-h"):
                case str2int("--help"):
                    show_usage(argv[0]);
                return 0;

                case str2int("-d"):
                case str2int("--debug"): {
                    DEBUG = true;
                    if (DEBUG) std::cout << "DEBUG activated" << std::endl;
                    break;
                }

                default:
                    std::cerr << "\nError: The \"" << arg
                              << "\" option doesn't exist. Write -h or --help to see the available options"
                              << std::endl;
                return 1;
            }


            }

        }


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

    }
