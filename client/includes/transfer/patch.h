//
// Created by fede on 10/26/20.
//

#ifndef CLIENT_PATCH_H
#define CLIENT_PATCH_H


#include <vector>
#include <string>
#include <chrono>
#include <map>

class Patch {
public:
    Patch(const std::filesystem::path mon_folder, std::vector<std::string> vector, std::vector<std::string> vector1, std::vector<std::string> vector2);
    std::vector<std::string> added_;
    std::map<std::string, unsigned long int> to_be_added_map_;
    std::vector<std::string> removed_;
    std::string to_be_deleted_;
    std::vector<std::string> common_;

    void PrettyPrint();

    std::filesystem::path monitored_folder_;
};


#endif //CLIENT_PATCH_H
