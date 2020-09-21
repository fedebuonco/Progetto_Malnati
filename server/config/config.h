//
// Created by marco on 21/09/20.
//

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>

class config {
    config()= default;

    static config* m_ConfigClass;

    void writeConfig(const std::string& username, const std::string& password);

public:
    config(const config&)= delete;
    config& operator=(const config&)=delete;

    static config* get_Instance();

    bool isConfig();
    void startConfig();
};


#endif //SERVER_CONFIG_H
