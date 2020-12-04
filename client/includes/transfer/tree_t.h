//
// Created by fede on 10/26/20.
//

#ifndef CLIENT_TREE_T_H
#define CLIENT_TREE_T_H


#include <string>
#include <map>
#include <filesystem>

/// The TreeT encapsulate a map containing for each element path a pair "element_path - last_modified_time"
class TreeT {
public:
    /// Map containing for each element its last modified time.
    std::map<std::string,unsigned long > map_tree_time_;
    TreeT(const std::filesystem::path& path);
    TreeT(const std::string& tree, const std::string& time);

};


#endif //CLIENT_TREE_T_H
