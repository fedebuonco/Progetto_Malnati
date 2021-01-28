#include "config.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <utilities.h>
#include <filesystem>
#include <regex>
#include <utility>

#include "cryptlib.h"
#include <sha.h>
#include <filters.h>
#include <hex.h>

//True means we are in debug, so we print more information in the console
bool DEBUG=false;

class SyntaxError : public std::exception {
private:
    std::string m_message;

public:
    explicit SyntaxError(std::string msg) : m_message(std::move(msg)) { }
    [[nodiscard]] const char * what () const noexcept override {
        return m_message.c_str();
    }
};


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
 * This function write a couple (Key:"Value") inside "config.json" file
 * @param key: JSON key
 * @param value: JSON value
 */
void Config::WriteProperty(const std::string& key, const std::string& value) {

    boost::property_tree::ptree  root;

    try {
        //Read the file and put the content inside root. If the file is wrong formatted, generate a pt::json_parser::json_parser_error.
        //If we don't do this all the information inside json file will be deleted
        std::filesystem::path config_file = this->exepath / "config_file" / "config.json";
        boost::property_tree::read_json(config_file.string(), root);

        //Create a node key, value
        root.put(key, value);

        //Overwrite the file with the root. If the file is wrong formatted, generate a pt::json_parser::json_parser_error.
        boost::property_tree::write_json(config_file.string(), root);
    }
    catch (const boost::property_tree::ptree_bad_path& e){
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr << "\nThe \"" << key << "\" was not found inside config.json" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    catch (const boost::property_tree::json_parser::json_parser_error& e) {
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr <<"\nThe configuration file was not found or is bad formatted" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/**
 * Read inside config JSON file and return the value associated with the given key.
 * @param  key: JSON key
 * @return the value associated with the given key or 'NULL' if we don't have any value.
 */
std::string Config::ReadProperty(const std::string &key) {

    boost::property_tree::ptree  root;

    try {
        //Read the file and put the content inside root. If the file is wrong formatted, generate a pt::json_parser::json_parser_error.
        std::filesystem::path config_file = this->exepath / "config_file" / "config.json";
        boost::property_tree::read_json(config_file.string(), root);

        //We get the value, if the key is not present, get() method will throw a pt::ptree_bad_path exception.
        auto value = root.get<std::string>(key);
        //auto value = root.get<std::string>(key, 'NULL'); //If you want to return default value.

        //If the value is not present but the key is present we retrieve NULL.
        if(value.empty()) value="NULL";

        return value;
    }
    catch (const boost::property_tree::ptree_bad_path& e){
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr << "\nThe \"" << key << "\" was not found inside config.json" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    catch (const boost::property_tree::json_parser::json_parser_error& e) {
        if(DEBUG) std::cerr << e.what() << std::endl;
        std::cerr <<"\nThe configuration file was not found or is bad formatted" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/**
 * This function takes the command line parameters and set config file appropriately
 * Throw an exception if the configuration syntax is wrongly formatted or the options are incorrect (e.x. wrong IPv4 structure).
 * @param argc: number of arguments
 * @param argv: list of arguments
 */
void Config::SetConfig(int argc, char *argv[]) {

    //First we check if the user starts the program with "-d" or "--debug" option to show also the debug information during the configuration.
    for (int i = 1; i < argc; ++i) {

        std::string arg = argv[i];

        if(arg=="-d" || arg=="--debug"){

            //We found the "-d" or "--debug" option inside the configuration options.

            //Then we check that the next argument has the '-' because we don't want that the user writes for example "-d debug".
            //If we have the next argument (i+1<argc) but the next argument doesn't starts with '-' we catch an exception.
            if (i + 1 < argc && (static_cast<std::string>(argv[i + 1]).rfind('-', 0) == std::string::npos)){
                //TODO: Exception
                throw  SyntaxError(std::string("Configuration syntax error: \"-d\" option doesn't want another argument. We found: ") + argv[i] + " " + argv[i+1]);
            }

            //We activate the debug prints and check with a print
            DEBUG = true;
            if(DEBUG) std::cout << "DEBUG activated correctly" << std::endl;
        }

    }

    //After setting the debug configuration if present, we set the other configuration options

    for (int i = 1; i < argc; ++i) {  //Start from 1 because argv[0] is the program name

        //Every time we do the cycle inside argv[i] we have the -option name (i.e. "-c") and not the option argument
        // (i.e. "username password") because we increment 'i' also inside the cycle every time we found an option.

        std::string arg = argv[i];

        //If the argument doesn't start with '-' there is a syntax problem
        //(i.e. "-c username password 127.0.0.1" instead of "-c username password -s 127.0.0.1")
        if(static_cast<std::string>(argv[i]).rfind('-', 0) == std::string::npos){
            //TODO: Exception
            throw  SyntaxError(std::string("Configuration syntax error: \"") + argv[i] + std::string("\" is not an option. Option must starts with \"-\", digit -h for more information."));
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

                    //We want to store the hashed password and username inside config file
                    CryptoPP::SHA256 hash;
                    std::string digest;

                    CryptoPP::StringSource s(password, true, new CryptoPP::HashFilter(
                            hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));

                    if(DEBUG) std::cout << "\nUser send the password: " << password << " and hash: " << digest << std::endl;


                    Config::get_Instance()->WriteProperty("username", username);
                    Config::get_Instance()->WriteProperty("hash_password", digest);

                } else {
                    //TODO: Exception
                    throw  SyntaxError(std::string("Configuration syntax error: We didn't find 2 arguments between the -c or --credential and the next option."));
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
                    //TODO: Exception
                    throw  SyntaxError(std::string("Configuration syntax error: We didn't find 1 valid arguments after the -f or --folder."));
                }

            }
                break;
            case str2int("-s"):
            case str2int("--server"): {

                if (i + 2 < argc && static_cast<std::string>(argv[i + 1]).rfind('-', 0) == std::string::npos &&
                                      static_cast<std::string>(argv[i + 2]).rfind('-', 0) == std::string::npos) {

                    std::string ip = argv[++i];
                    std::string port = argv[++i];

                    /**
                     * The two following expression are regex for ipv4 address and port
                     * The IPv4 regex say that the structure must be: number1.number2.number3.number4 and each number go from 0 to 255.
                     * The port address must be a number with max 5 digits
                     */

                    std::regex pattern_ip("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])" );
                    std::regex pattern_port("\\d{1,5}");

                    if (!std::regex_match (ip, pattern_ip ) || !std::regex_match (port, pattern_port )) {
                        //TODO: Exception
                        throw  SyntaxError(std::string("Wrong IPv4 address or port format"));
                    }

                    Config::get_Instance()->WriteProperty("ip", ip);
                    Config::get_Instance()->WriteProperty("port", port);

                } else {
                    //TODO: Exception
                    throw  std::invalid_argument("Configuration syntax error: We didn't find 2 valid arguments after the -s or --server.");
                    //throw  SyntaxError(std::string("Configuration syntax error: We didn't find 2 valid arguments after the -s or --server."));
                }

                break;

            }
            case str2int("-h"):
            case str2int("--help"):
                show_usage(argv[0]);
                //TODO EXIT Verificare se è corretto
                //TODO Verificare se fare tipo di eccezione speciale e fare chiudere al main
                std::exit(0);
               

            case str2int("-d"):
            case str2int("--debug"): {

                //We go next because the debug configuration is already made
                break;
            }

            //If we arrive in the default option, it means that the option doesn't exist
            default:
                //TODO: Exception
                throw  SyntaxError(std::string("Configuration syntax error: \"") + argv[i] + std::string("\" option doesn't exist. Write -h or --help to see the available options."));
        }

    }

}


/**
 * Print program configuration inside the console
 */
void Config::PrintConfiguration() {

    std::string username = Config::get_Instance()->ReadProperty("username");
    std::string backup_folder = Config::get_Instance()->ReadProperty("path");
    std::string ip = Config::get_Instance()->ReadProperty("ip");
    std::string port = Config::get_Instance()->ReadProperty("port");

    std::cout   << "\n\nProgram started with:\n"
                << "\t Username: \t" << username << "\n"
                << "\t Backup folder: " << backup_folder << "\n"
                << "\t Ip and port: \t" << ip << " " << port <<"\n"
                << "\t Debug: \t" << std::boolalpha << DEBUG << "\n"
                << std::endl;
}

/**
 * This function will found the absolute path that points to client folder
 * @param your_path : This the path with which the executable is launched
 */
void Config::SetPath(const std::string& your_path) {
    // The executable will be placed in the /bin being / the root of our project. ( so at the same level of /libs, /includes , etc)
    // so in order to find the config folder and file we need to navigate to that folder
    //std::cout << " STRING PASSED : " << your_path << std::endl;

    std::filesystem::path executable_path = std::filesystem::path(your_path).lexically_normal();
    //std::cout <<" executable_path : "<< executable_path.string() << std::endl;

    // we get the absolute path
    std::filesystem::path executable_path_abs = std::filesystem::absolute(executable_path);
    //std::cout <<" executable_path_abs : "<< executable_path_abs.string() << std::endl;

    std::filesystem::path bin_path_abs = executable_path_abs.remove_filename();
    //std::cout <<" bin_path_abs : "<< bin_path_abs.string() << std::endl;

    std::filesystem::path master_path_abs = bin_path_abs.parent_path().parent_path();
    //std::cout <<" master_path_abs : "<< master_path_abs.string() << std::endl;

    std::cout << "Path \"" << your_path << "\" converted into \"" << master_path_abs.string() << "\"." << std::endl;

    this->exepath = master_path_abs;
}


/// Reads the raw endpoint from the json file.
/// \return A RawEndpoint used later to set up all the sockets.
RawEndpoint Config::ReadRawEndpoint() {

    std::string ip = Config::get_Instance()->ReadProperty("ip");

    //We convert the string in unsigned long
    auto port = std::stoul( Config::get_Instance()->ReadProperty("port"));

    return RawEndpoint{ip, port};
}

/**
 * It checks if the config structure (file and folder) have a folder 'config_file' with inside 'config.json' file.
 *
 * @return 'true' if structure is correct, otherwise 'false'.
 */
bool Config::IsConfigStructureCorrect() {

    std::filesystem::path config_file_path{ "../config_file/config.json" };
    std::filesystem::directory_entry config_directory_path{"../config_file"};

    //Function exists check if the given file path corresponds to an existing file or directory.
    //We need also to check that 'config.json' is a file and that 'config_file' is a folder

    if(      !(   std::filesystem::exists(config_directory_path) && std::filesystem::is_directory(config_directory_path)  )
             ||  !(   std::filesystem::exists(config_file_path) && !std::filesystem::is_directory(config_file_path))

       ){
        return false;
    }
    return true;
}

/**
 * It resets all the fields inside configuration file (config.json) with default value: <br>
 *      - Default Endpoint:         127.0.0.1 3333      <br>
 *      - Username and password:    empty               <br>
 *      - Folder:                   empty               <br>
 * <br>
 * It also reset the configuration structure (create the correct folder and file) if it is broken for some reason (i.e. file or folder doesn't exist). <br>
 *
 * This function must be used when the program detect problem during configuration phase. In this way we restore all with the default files.
 */
void Config::SetDefaultConfig() {

    if(DEBUG) std::cout << "Restore default configuration of config file and structure" << std::endl;

    //First of all we delete the structure
    std::filesystem::remove_all("../config_file");

    //Now we create the 'config_file' folder and the config.json file
    std::filesystem::create_directories("../config_file");
    std::ofstream config_file ("../config_file/config.json");

    //Fill the config.json file with the correct structure
    config_file << "{\n"
                   "    \"ip\": \"127.0.0.1\",\n"
                   "    \"port\": \"3333\",\n"
                   "    \"username\": \"\",\n"
                   "    \"hash_password\": \"\",\n"
                   "    \"path\": \"\"\n"
                   "}";

    config_file.close();

}













