
#pragma once

#include <string>
#include <filesystem>

class Database {

public:
    bool auth(std::string username, std::string hashpassword, std::filesystem::path serverP);

    std::string getUserPath(std::string username, std::filesystem::path serverP);

    void createTable(std::string foldername, std::filesystem::path serverP);

    std::string getTimefromPath(std::string foldername, std::string path, std::filesystem::path serverP);

};



