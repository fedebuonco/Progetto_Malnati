//
// Created by marco on 21/09/20.
//

#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>

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
    int SetConfig2(int argc, char *argv[]);

    void PrintConfiguration();



    std::string readUsername();
    void writeConfig(const std::string& username, const std::string& password);

    RawEndpoint ReadRawEndpoint();
    void WriteRawEndpoint(const std::string& ip, const std::string& port);

    std::string ReadFolderPath();
    void WriteFolderPath(const std::string& path);

    bool isConfig();
    void startConfig();
};


#endif //SERVER_CONFIG_H
