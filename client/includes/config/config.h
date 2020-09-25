//
// Created by marco on 21/09/20.
//

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>

struct Connection{
    std::string raw_ip_address;
    unsigned short port_num;
};

class Config {
    Config()= default;

    static Config* m_ConfigClass;

    void writeConfig(const std::string& username, const std::string& password);

public:
    Config(const Config&)= delete;
    Config& operator=(const Config&)=delete;

    static Config* get_Instance();

    Connection ReadConnection();

    bool isConfig();
    void startConfig();
};


#endif //SERVER_CONFIG_H
