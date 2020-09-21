//
// Created by marco on 21/09/20.
//

#include "ConfigClass.h"
#include "../src/utilities/ut.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

bool ConfigClass::isConfig() {

    namespace pt = boost::property_tree;

    //This is the tree root; inside there is the username and password (if the app is config)
    pt::ptree  root;

    //Read the file and put the content inside root
    pt::read_json("../config/credential.json", root);

    auto username = root.get<std::string>("username");
    auto password = root.get<std::string>("password");

    if (DEBUG) std::cout << "Inside config there is username: " << username<< " and password: " << password << std::endl;

    if(username=="NULL" || password=="NULL"){
        //This means that the user is not legged in
        return false;
    } else {
        return true;
    }
}

/**
 * Asks the user to authenticate (username and password).
 * Next asks the server if the credential is correct.
 * If is correct save the credential inside the JSON file.
 * */
void ConfigClass::startConfig() {

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


    //Send to server username and password and check if is correct
    //TO DO
    //VEDERE SE FARLO QUI O FAR RIMANDARE INDIETRO ALLA FUNZIONE USERNAME AND PASSWORD


    //Write the username and password inside JSON file
    writeConfig(username, password);

}

/**
 * Write the username and password inside JSON file
 **/
void ConfigClass::writeConfig(const std::string& username, const std::string& password) {

    namespace pt = boost::property_tree;

    //This is the tree root; inside there is the username and password (if the app is config)
    pt::ptree  root;

    root.put("username", username);
    root.put("password", password);

    //Read the file and put the content inside root
    pt::write_json("../config/credential.json", root);

}
