#include "config.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <utilities.h>
#include <filesystem>


#include <sha.h>
#include <filters.h>
#include <hex.h>
#include "cryptlib.h"


bool DEBUG=false;

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
        std::cerr << "The \"" << key << "\" was not found inside config.json" << std::endl;

        std::exit(23);   //TODO: Check the error status

    }
    catch (const boost::property_tree::json_parser_error& e1) {
        std::cerr <<"The configuration file was not found. Check if there is config.json" << std::endl;

        std::exit(12);   //TODO: Check the error status
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

                    CryptoPP::SHA256 hash;
                    std::string digest;

                    CryptoPP::StringSource s(password, true, new CryptoPP::HashFilter(
                            hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));

                    if(DEBUG) std::cout << "\nUser send the password: " << password << " and hash: " << digest << std::endl;


                    Config::get_Instance()->WriteProperty("username", username);
                    Config::get_Instance()->WriteProperty("hash_password", digest);

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

                    //I take the path with the OS standard (Windows .\ while Linux ./)
                    std::filesystem::path path = std::filesystem::path( argv[++i] );

                    //Convert the path into the POSIX standard (./)
                    std::string path_string = path.generic_string();

                    Config::get_Instance()->WriteProperty("path", path_string);

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
                //TODO Verificare se è corretto
                std::exit(0);
               

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
                << "\t Backup folder: " << Config::get_Instance()->ReadProperty("path") << "\n"
                << "\t Ip and port: \t" << Config::get_Instance()->ReadProperty("ip") << " " << Config::get_Instance()->ReadProperty("port") <<"\n"
                << "\t Debug: \t" << std::boolalpha << DEBUG << "\n"
                << std::endl;

}



/// Reads the raw endpoint from the json file.
/// \return A RawEndpoint used later to set up all the sockets.
RawEndpoint Config::ReadRawEndpoint() {

    std::string ip = Config::get_Instance()->ReadProperty("ip");

    //We convert the string in unsigned long
    auto port = std::stoul( Config::get_Instance()->ReadProperty("port"));

    return RawEndpoint{ip, port};
}












