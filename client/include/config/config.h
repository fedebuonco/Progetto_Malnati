//
// Created by marco on 21/09/20.
//

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>

class Config {
    Config()= default;

    static Config* m_ConfigClass;

    void writeConfig(const std::string& username, const std::string& password);

public:
    Config(const Config&)= delete;
    Config& operator=(const Config&)=delete;

    static Config* get_Instance();

    bool isConfig();
    void startConfig();
};


#endif //SERVER_CONFIG_H
