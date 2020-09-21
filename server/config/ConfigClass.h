//
// Created by marco on 21/09/20.
//

#ifndef SERVER_CONFIGCLASS_H
#define SERVER_CONFIGCLASS_H


#include <string>

class ConfigClass {
    static void writeConfig(const std::string& username, const std::string& password);

public:
    static bool isConfig();
    static void startConfig();
};


#endif //SERVER_CONFIGCLASS_H
