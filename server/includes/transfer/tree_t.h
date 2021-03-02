#pragma once

#include <string>
#include <map>
#include <filesystem>

class TreeT {

public:
    ///String of the tree, ready to be serialized and sent.
    std::string tree_;
    ///Path of the folder
    std::filesystem::path folder_path_;
    /// Map of files and lmt
    std::map<std::string,unsigned long > map_tree_time_;
    /// Path of the server.exe
    std::filesystem::path serverPath;

    TreeT(const std::filesystem::path& path, const std::filesystem::path& serverP);
    TreeT(const std::string& tree, const std::string& time);
    std::string genTree();
    std::string genTimes();
};
