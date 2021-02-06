#pragma once


#include <string>
#include <map>
#include <filesystem>

/// The TreeT encapsulate a map containing for each element path a pair "element_path - last_modified_time"
class TreeT {

public:
    /// Map containing for each file path its last modified time.
    std::map<std::string,unsigned long > map_tree_time_;
    //TODO: Modified to make private? @marco

     explicit TreeT(const std::filesystem::path& path);
    TreeT(const std::string& tree, const std::string& time);

};

