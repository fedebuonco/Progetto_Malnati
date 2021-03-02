#pragma once

#include <string>
#include <filesystem>
#include <mutex>
#include <shared_mutex>

/**
 * Class that manage the connection and operation with the DBs
 */
class Database {

    ///Mutex that protected the access to DBs in order to handle mutual exclusion
    static std::shared_mutex db_mutex_;

public:

    bool auth(const std::string& username, const std::string& attempt_hash_password, const std::filesystem::path& serverP);

    std::string getUserPath(const std::string& username, const std::filesystem::path& serverP);

    void createTable(const std::string& folderName, const std::filesystem::path& serverP);

    std::string getTimeFromPath(const std::string& folderName, const std::string& path, const std::filesystem::path& serverP);

    static void insertFile(const std::string& userName, const std::string& pathName, const std::string& lmt, const std::filesystem::path& serverP);
    void deleteFile(const std::string& folderName, const std::string& path, const std::filesystem::path& serverP);
};



