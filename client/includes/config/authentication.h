#pragma once

#include <string>


struct Credential{
    std::string username_;
    std::string hash_password_;
};


/**
 * Class that handle the user authentication
 */
class Authentication {

    static Authentication* m_AuthenticationClass;
    Authentication() = default;

public:
    Authentication(const Authentication&)= delete;
    Authentication& operator=(const Authentication&)=delete;

    // Used for getting the static instance of the Authentication class, part of the singleton pattern.
    static Authentication* get_Instance();

    Credential ReadCredential();

};

