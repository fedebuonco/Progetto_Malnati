//
// Created by fede on 12/24/20.
//
#pragma once

#include <string>
#include <SQLiteCpp/Database.h>
#include <iostream>
#include <filesystem>

class DatabaseConnection{
    std::filesystem::path folder_watched_;
    std::filesystem::path db_path;
    SQLite::Database hash_db_;

public:

    DatabaseConnection(std::filesystem::path db_path, std::filesystem::path folder_watched);
    bool AlreadyHashed(std::string filename, std::string lmt);
    void InsertDB(std::string path_str, std::string hash, std::string lmt_str);
    void CleanOldRows();

    bool ChangeStatusToSending(const std::string& filename);
};

