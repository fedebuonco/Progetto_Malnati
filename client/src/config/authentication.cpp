//
// Created by marco on 25/09/2020.
//

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
// #include <utilities.h>
#include <config.h>
#include <iostream>
#include "authentication.h"

#define CREDENTIAL_PATH "../config_file/credential.json"


Authentication *Authentication::m_AuthenticationClass = nullptr;

Authentication *Authentication::get_Instance() {

    if(!m_AuthenticationClass){
        m_AuthenticationClass = new Authentication;
    }
    return m_AuthenticationClass;
}


///----------------------------------------------------------------



Credential Authentication::ReadCredential() {

    namespace pt = boost::property_tree;

    //This is the tree root; inside there is the username and password (if the app is config)
    pt::ptree root;

    try {
        //Read the file and put the content inside root
        pt::read_json(CREDENTIAL_PATH, root);

        auto username = root.get<std::string>("username");
        auto password = root.get<std::string>("password");

        if (DEBUG)
            std::cout << "Inside config there is username: " << username << " and password: " << password << std::endl;

        return Credential{username, password};
    }
    catch (const boost::property_tree::ptree_bad_path& e2){
        std::cerr << "The configuration file has a wrong structure: it must have a 'Username' and 'Password' field" << std::endl;

        std::exit(23);   //TODO: Check the error status

    }
    catch (const boost::property_tree::json_parser_error& e1) {
        std::cerr <<"The configuration file was not found" << std::endl;

        std::exit(12);   //TODO: Check the error status
    }

}


/**
 * Check if the credentials have been entered correctly
 * @param credential: Credential struct (Username and Password)
 * @return true if there are valid credential
 */
bool Authentication::IsValidCredential(const Credential& credential) {

    if(credential.username_=="NULL" || credential.password_=="NULL"){
        //This means that the user is not legged in
        return false;
    } else {
        return true;
    }
}




