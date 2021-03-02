#pragma once
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <string>
#include <filesystem>

//Any source file that includes this will be able to use "DEBUG"
//The value is define inside config.cpp
extern bool DEBUG;

/// Encapsulates a raw ip and a port.
struct RawEndpoint{
    /// Raw ip as a string, will be parsed.
    std::string raw_ip_address;
    /// Raw port number.
    unsigned long port_num;
};



///Class that handle the configuration phase
class Config {
    Config()= default;

    ///Class pointer used for implementing singleton pattern
    static Config* m_ConfigClass;

    ///Path of the user's client exe
    std::filesystem::path exe_path;

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
    void SetPath(const std::string& your_path);

    void PrintConfiguration();

    RawEndpoint ReadRawEndpoint();
};

