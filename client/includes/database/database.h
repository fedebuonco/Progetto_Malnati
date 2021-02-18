#pragma once

#include <string>
#include <SQLiteCpp/Database.h>
#include <iostream>
#include <filesystem>
#include <mutex>
#include <shared_mutex>
#include <vector>

class DatabaseConnection{
    std::filesystem::path folder_watched_;
    SQLite::Database hash_db_;

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
};

