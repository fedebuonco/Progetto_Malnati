
#pragma once

#include <string>

class Database {

public:
    bool auth(std::string username, std::string hashpassword);

    std::string getUserPath(std::string username);

    void createTable(std::string foldername);

};



