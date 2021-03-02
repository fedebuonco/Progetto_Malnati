#pragma once

#include <string>

/**
 * Struct used for authentication. Made of username and hash_pass
 */
struct Credential{
    ///Username used for authentication
    std::string username_;
    ///Password provided by the user. We stored the hash.
    std::string hash_password_;
};


/**
 * Class that handle the user authentication
 */
class Authentication {

    //Class pointer used for singleton pattern
    static Authentication* m_AuthenticationClass;
    Authentication() = default;

public:
    Authentication(const Authentication&)= delete;
    Authentication& operator=(const Authentication&)=delete;

    // Used for getting the static instance of the Authentication class, part of the singleton pattern.
    static Authentication* get_Instance();

    Credential ReadCredential();

};

