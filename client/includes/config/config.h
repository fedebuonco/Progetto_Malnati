//
// Created by marco on 21/09/20.
//

#pragma once
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <string>
#include <filesystem>

//Any source file that includes this will be able to use "DEBUG"
//The value is define inside config.cpp
//TODO CAPIRE MEGLIO https://stackoverflow.com/questions/10422034/when-to-use-extern-in-c/10422050#10422050
extern bool DEBUG;

/// Encapsulates a raw ip and a port.
struct RawEndpoint{
    /// Raw ip as a string, will be parsed.
    std::string raw_ip_address;
    /// Raw port number.
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

    bool IsConfigStructureCorrect();
    void SetDefaultConfig();

    void SetConfig(int argc, char *argv[]);

    void PrintConfiguration();

    void SetPath(std::string s);

    RawEndpoint ReadRawEndpoint();

private:
    std::filesystem::path exepath;

};

