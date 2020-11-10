//
// Created by marco on 21/09/20.
//

#pragma once

#include <string>

//Any source file that includes this will be able to use "DEBUG"
//The value is define inside utilities.cpp
//TODO CAPIRE MEGLIO https://stackoverflow.com/questions/10422034/when-to-use-extern-in-c/10422050#10422050
extern bool DEBUG;

/// Encapsulates a raw ip and a port.
struct RawEndpoint{
    std::string raw_ip_address;
    unsigned long port_num;
};



///
class Config {
    Config()= default;
    static Config* m_ConfigClass;

public:
    //Singleton, eliminate copy and assignment
    Config(const Config&)= delete;
    Config& operator=(const Config&)=delete;

    static Config* get_Instance();

    void WriteProperty(const std::string& key, const std::string& value);
    std::string ReadProperty(const std::string& key);

    int SetConfig(int argc, char *argv[]);

    void PrintConfiguration();

    RawEndpoint ReadRawEndpoint();

};

