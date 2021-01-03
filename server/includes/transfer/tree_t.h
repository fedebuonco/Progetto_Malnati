//
// Created by fede on 10/26/20.
//

#ifndef CLIENT_TREE_T_H
#define CLIENT_TREE_T_H


#include <string>
#include <map>
#include <filesystem>

class TreeT {
public:
    std::string tree_;
    std::filesystem::path folder_path_;
    std::map<std::string,unsigned long > map_tree_time_;
    std::filesystem::path serverPath;
    TreeT(const std::filesystem::path& path, std::filesystem::path serverP);
    TreeT(const std::string& tree, const std::string& time);

    std::string genTree();

    std::string genTimes();
};


#endif //CLIENT_TREE_T_H
