
#pragma once

#include <string>

class Database {

public:
    bool auth(std::string username, std::string hashpassword, std::string serverPath);

    std::string getUserPath(std::string username, std::string serverPath);

    void createTable(std::string foldername, std::string serverPath);

    std::string getTimefromPath(std::string foldername, std::string path, std::string serverPath);

};



