#include "config.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <utilities.h>
#include <authentication.h>

#define CREDENTIAL_PATH "../config_file/credential.json"

Config *Config::m_ConfigClass = nullptr;

/// Assures that only one instance is present.
/// \return the instance of config class
Config *Config::get_Instance() {

    if(!m_ConfigClass){
        m_ConfigClass = new Config;
    }
    return m_ConfigClass;
}

/**
 * Check if the configuration file has username and password inside
 * Return true if is valid while false if is NULL
 * */
bool Config::isConfig() {

        //Take the use credential and save it
        Credential credential_ = Authentication::get_Instance()->ReadCredential();

        //Check if there aren't valid credential
        return Authentication::get_Instance()->IsValidCredential(credential_);

}

/** TODO DA CANCELLARE
 * Asks the user to authenticate (username and password) and save the credential inside the JSON file.
 * */
void Config::startConfig() {

    std::string username;
    std::string password;

    std::cout << "It turns out that you are not logged id.\nPlease provide a username and password." << std::endl;

    do {
        std::cin.clear(); //Clear cin if there was some error during the previous loop

        std::cout << "Username: ";
        std::cin >> username;

        std::cout << "Password: ";
        std::cin >> password;

    }while ( !std::cin.good() );
    //PER ORA CHIEDE ALL'INFINITO SE NON PRENDE BENE I DATI, VALUTARE SE FARE CHE DOPO TOT PROVE IL PROGRAMMA TERMINA CON UN ERRORE
    //VEDERE SU INTERNET SE TROVIAMO CASI IN CUI IL CIN DA ERRORE COSI DA TESTARLO

    if(DEBUG) std::cout << "You inserted " << username << " " << password << std::endl;

    //Write the username and password inside JSON file
    writeConfig(username, password);

}

/**
 * This function write Key, Value inside config JSON file
 * @param key: JSON key
 * @param value: JSON value
 */
void Config::WriteProperty(const std::string& key, const std::string& value) {

    namespace pt = boost::property_tree;

    pt::ptree  root;

    // Load the json file in this ptree
    // If we don't do this all the information inside json file will be deleted
    // TODO gestire errori nella lettura del json
    pt::read_json("../config_file/config.json", root);

    try {
        root.put(key, value);

        //Read the file and put the content inside root
        pt::write_json("../config_file/config.json", root);
    }
    catch ( const boost::property_tree::json_parser_error& e1) {
        std::cerr <<"The configuration file was not found" << std::endl;

        std::exit(12);   //TODO: Check the error status
    }
}

/**
 * Read inside config JSON file and return the value associated with the given key
 * @param key: JSON key
 * @return the value associated with the given key
 */
std::string Config::ReadProperty(const std::string &key) {
    namespace pt = boost::property_tree;
    pt::ptree  root;

    try {
        // TODO gestire errori nella lettura del json
        //Read the file and put the content inside root
        pt::read_json("../config_file/config.json", root);

        auto value = root.get<std::string>(key);

        return value;
    }
    catch (const boost::property_tree::ptree_bad_path& e2){
        std::cerr << "The " << key << " was not found" << std::endl;

        std::exit(23);   //TO-DO: Check the error status

    }
    catch (const boost::property_tree::json_parser_error& e1) {
        std::cerr <<"The configuration file was not found" << std::endl;

        std::exit(12);   //TO-DO: Check the error status
    }
}

/**
 * This function takes the command line parameters and set config file appropriately
 * @param argc: number of arguments
 * @param argv: list of arguments
 * @return 0 (no problem) or 1 (problems)
 */
int Config::SetConfig(int argc, char *argv[]) {

    for (int i = 1; i < argc; ++i) {  //Start from 1 because argv[0] is the program name

        std::string arg = argv[i];

        if(static_cast<std::string>(argv[i]).rfind('-', 0) == std::string::npos){
            std::cerr << "Syntax error: \"" << argv[i] << "\" is not an option. Option must starts with \"-\", digit -h for more information." << std::endl;
            return 1;
        }

        //We need to use 'str2int' function because switch cannot take string but only integer
        switch (str2int(arg.c_str())) {

            case str2int("-c"):
            case str2int("--credential"): {

                //We check first if there are other 2 arguments after "-c" (username and password)
                //Then we check if this two arguments are not options (prevent the case "-c username -h")
                if (i + 2 < argc && static_cast<std::string>(argv[i + 1]).rfind('-', 0) == std::string::npos &&
                                      static_cast<std::string>(argv[i + 2]).rfind('-', 0) == std::string::npos) {

                    std::string username = argv[++i];
                    std::string password = argv[++i];

                    Config::get_Instance()->WriteProperty("username", username);
                    Config::get_Instance()->WriteProperty("password", password);

                } else {
                    std::cerr
                            << "Syntax error: We didn't find 2 arguments between the -c or --credential and the next option"
                            << std::endl;
                    return 1;
                }

                break;
            }
            case str2int("-f"):
            case str2int("--folder"): {

                if (i + 1 < argc && static_cast<std::string>(argv[i + 1]).rfind('-', 0) == std::string::npos){

                    std::string path = argv[++i];

                    Config::get_Instance()->WriteProperty("path", path);

                } else {
                    std::cerr
                            << "Syntax error: We didn't find 1 valid arguments after the -f or --folder"
                            << std::endl;
                    return 1;
                }

            }
                break;
            case str2int("-s"):
            case str2int("--server"): {

                if (i + 2 < argc && static_cast<std::string>(argv[i + 1]).rfind('-', 0) == std::string::npos &&
                                      static_cast<std::string>(argv[i + 2]).rfind('-', 0) == std::string::npos) {

                    std::string ip = argv[++i];
                    std::string port = argv[++i];

                    Config::get_Instance()->WriteProperty("ip", ip);
                    Config::get_Instance()->WriteProperty("port", port);

                } else {
                    std::cerr
                            << "Syntax error: We didn't find 2 valid arguments after the -s or --server"
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


                //We don't want that user write "-d debug"
                if (i + 1 < argc && (static_cast<std::string>(argv[i + 1]).rfind('-', 0) == std::string::npos)){
                    std::cerr
                            << "Syntax error: -d don't want another argument"
                            << std::endl;
                    return 1;
                }

                    DEBUG = true;
                break;
            }

            default:
                std::cerr << "\nError: The \"" << arg
                          << "\" option doesn't exist. Write -h or --help to see the available options"
                          << std::endl;
                return 1;
        }

    }
    return 0;
}


/**
 * Print program configuration inside the console
 */
void Config::PrintConfiguration() {

    std::cout   << "\n\nProgram started with:\n"
                << "\t Username: \t" << Config::get_Instance()->ReadProperty("username")<< "\n"
                << "\t Folder: \t" << Config::get_Instance()->ReadProperty("path") << "\n"
                << "\t Ip and port: \t" << Config::get_Instance()->ReadProperty("ip") << " " << Config::get_Instance()->ReadProperty("port") <<"\n"
                << "\t Debug: \t" << std::boolalpha << DEBUG << "\n"
                << std::endl;

}


/**
 * Write the username and password inside JSON file
 **/
void Config::writeConfig(const std::string& username, const std::string& password) {

    namespace pt = boost::property_tree;

    //This is the tree root; inside there is the username and password (if the app is config)
    pt::ptree  root;

    try {
        root.put("username", username);
        root.put("password", password);

        //Read the file and put the content inside root
        pt::write_json(CREDENTIAL_PATH, root);
    }
    catch ( const boost::property_tree::json_parser_error& e1) {
        std::cerr <<"The configuration file was not found" << std::endl;

        std::exit(12);   //TO-DO: Check the error status
    }
}


/// Reads the raw endpoint from the json file.
/// \return A RawEndpoint used later to set up all the sockets.
RawEndpoint Config::ReadRawEndpoint() {

    std::string ip = Config::get_Instance()->ReadProperty("ip");

    //We convert the string in unsigned long
    auto port = std::stoul( Config::get_Instance()->ReadProperty("port"));

    return RawEndpoint{ip, port};


}

//TODO Mettere un po' di controlli in modo tale che la porta e l'ip siano corretti
void Config::WriteRawEndpoint(const std::string &ip, const std::string& port) {

        namespace pt = boost::property_tree;

        //This is the tree root; inside there is the username and password (if the app is config)
        pt::ptree  root;

        try {
            root.put("ip", ip);
            root.put("port", port);

            //Read the file and put the content inside root
            pt::write_json("../config_file/connection.json", root);
        }
        catch ( const boost::property_tree::json_parser_error& e1) {
            std::cerr <<"The configuration file was not found" << std::endl;

            std::exit(12);   //TO-DO: Check the error status
        }
}

void Config::WriteFolderPath(const std::string &path) {

    namespace pt = boost::property_tree;

    //This is the tree root; inside there is the username and password (if the app is config)
    pt::ptree  root;

    try {
        root.put("path", path);

        //Read the file and put the content inside root
        pt::write_json("../config_file/folder.json", root);
    }
    catch ( const boost::property_tree::json_parser_error& e1) {
        std::cerr <<"The configuration file was not found" << std::endl;

        std::exit(12);   //TO-DO: Check the error status
    }


}

/// Reads the raw endpoint from the json file.
/// \return A RawEndpoint used later to set up all the sockets.
std::string Config::ReadFolderPath() {

    namespace pt = boost::property_tree;
    pt::ptree  root;

    try {
        // TODO gestire errori nella lettura del json
        //Read the file and put the content inside root
        pt::read_json("../config_file/folder.json", root);

        auto path = root.get<std::string>("path");


        return path;
    }
    catch (const boost::property_tree::ptree_bad_path& e2){
        std::cerr << "The configuration file has a wrong structure: it must have a 'Username' and 'Password' field" << std::endl;

        std::exit(23);   //TO-DO: Check the error status

    }
    catch (const boost::property_tree::json_parser_error& e1) {
        std::cerr <<"The configuration file was not found" << std::endl;

        std::exit(12);   //TO-DO: Check the error status
    }

}

std::string Config::readUsername() {
    namespace pt = boost::property_tree;
    pt::ptree  root;

    try {
        // TODO gestire errori nella lettura del json
        //Read the file and put the content inside root
        pt::read_json("../config_file/credential.json", root);

        auto username = root.get<std::string>("username");


        return username;
    }
    catch (const boost::property_tree::ptree_bad_path& e2){
        std::cerr << "The configuration file has a wrong structure: it must have a 'Username' and 'Password' field" << std::endl;

        std::exit(23);   //TO-DO: Check the error status

    }
    catch (const boost::property_tree::json_parser_error& e1) {
        std::cerr <<"The configuration file was not found" << std::endl;

        std::exit(12);   //TO-DO: Check the error status
    }
}










