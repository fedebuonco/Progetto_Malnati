#include "config.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include <utilities.h>
#include <authentication.h>

#define CREDENTIAL_PATH "../config_file/credential.json"

Config *Config::m_ConfigClass = nullptr;

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

/**
 * Asks the user to authenticate (username and password) and save the credential inside the JSON file.
 * */
void Config::startConfig() {

    std::string username;
    std::string password;

    std::cout << "It turns out that you are not legged id.\nPlease provide a username and password." << std::endl;

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

Connection Config::ReadConnection() {

    namespace pt = boost::property_tree;

    pt::ptree  root;

    try {
        // TODO gestire errori nella lettura del json
        //Read the file and put the content inside root
        pt::read_json("../config_file/connection.json", root);


        auto raw_ip_address = root.get<std::string>("ip");
        auto port_num = root.get<unsigned short>("port");

        //TODO pensare a quando è valido o no ip e port
        if( raw_ip_address=="NULL" || port_num==0){
            std::cerr << "Connection file error" << std::endl;
            //TODO Throw error and catch sotto
            //throw std::exception();
        }

        return Connection{raw_ip_address, port_num};

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



