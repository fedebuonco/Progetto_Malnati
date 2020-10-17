//
// Created by marco on 21/09/20.
//

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>

/// Encapsulates a raw ip and a port.
struct RawEndpoint{
    std::string raw_ip_address;
    unsigned short port_num;
};

///
class Config {
    Config()= default;
    static Config* m_ConfigClass;

public:
    //Singleton, eliminate copy and assignment
    Config(const Config&)= delete;
    Config& operator=(const Config&)=delete;

    void writeConfig(const std::string& username, const std::string& password);

    static Config* get_Instance();

    RawEndpoint ReadRawEndpoint();
    void WriteRawEndpoint(const std::string& ip, const std::string& port);

    void WriteFolderPath(const std::string& path);

    bool isConfig();
    void startConfig();
};


#endif //SERVER_CONFIG_H
