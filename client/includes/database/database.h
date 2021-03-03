#pragma once

#include <string>
#include <SQLiteCpp/Database.h>
#include <iostream>
#include <filesystem>
#include <mutex>
#include <shared_mutex>
#include <vector>

/**
 * Class that manage the connection and operation with the DB
 */
class DatabaseConnection{

    ///The user's folder to be monitored
    std::filesystem::path folder_watched_;

    ///Database object used for connection
    SQLite::Database hash_db_;

    ///Mutex that protected the access to DB in order to handle it in mutual exclusion
    static std::shared_mutex db_mutex_;


public:
    DatabaseConnection(const std::filesystem::path& db_path, std::filesystem::path folder_watched);
    bool AlreadyHashed(const std::string& filename, const std::string& lmt);
    void InsertDB(const std::string& path_str, const std::string& hash, const std::string& lmt_str);
    void GetMetadata(const std::string& filename, std::string& hash, std::string& lmt);
    void CleanOldRows();
    int  SetBackToNew();
    bool ChangeStatusToSending(const std::string& filename);
    bool ChangeStatusToSent(const std::string& filename);
    bool ChangeStatusToNew(const std::string& filename);
    bool ChangeStatusToNotSent(const std::string& filename);
    bool AlignStatus(const std::vector<std::pair<std::string, unsigned  long>>& sfileslmt);
    bool AllSent();
};

