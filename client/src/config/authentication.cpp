//
// Created by marco on 25/09/2020.
//

#include <boost/property_tree/json_parser.hpp>
#include <config.h>
#include <iostream>
#include "authentication.h"


Authentication *Authentication::m_AuthenticationClass = nullptr;

Authentication *Authentication::get_Instance() {

    if(!m_AuthenticationClass){
        m_AuthenticationClass = new Authentication;
    }
    return m_AuthenticationClass;

}

/**
 * This function read the username and hash_password stored inside config.json
 * @return Credential (a struct with username and password)
 */
Credential Authentication::ReadCredential() {

    std::string username = Config::get_Instance()->ReadProperty("username");
    std::string hash_password = Config::get_Instance()->ReadProperty("hash_password");


    return Credential{username, hash_password};
}


