#include <sstream>
#include "tree_t.h"
#include "../../includes/database/database.h"
#include <vector>   //For Linux
#include <algorithm>
#include <iostream>

/// Starting from the ordered string sent by the server, it computes A mapping between the files present in te server and their hashes
/// \param tree Directory tree if the server
/// \param time time of the files present in the server following the same order of the tree
TreeT::TreeT(const std::string& tree, const std::string& time) {
    //Now we fill the map using the two string we got
    std::istringstream stream_tree{tree};
    std::istringstream stream_time{time};
    std::string filename;
    std::string file_time;
    while ( (std::getline(stream_tree, filename)))
    {
        if (std::getline(stream_time, file_time)) {
            map_tree_time_.insert(std::pair<std::string, unsigned long>(filename, std::stoul(file_time, nullptr, 0)));
        } else {
            map_tree_time_.insert(std::pair<std::string, unsigned long>(filename, 0));
        }
    }
}

/**
 * This function create the tree & time from a given path relatively to the path of the user Server folder
 * @param path : user backup folder in Server
 * @param serverP : location of the Server program
 */
TreeT::TreeT(const std::filesystem::path& path, const std::filesystem::path& serverP){

    this->serverPath = serverP;
    this->folder_path_ = path;
    int gen_attempts = 10;

    std::filesystem::path folder_name = path.lexically_relative(this->serverPath / "backupFiles" / "backupROOT");
    std::string folder_name_string = folder_name.generic_string();

    // We try this generation for x attempts, this is because some folder could be deleted while we
    // gen the treet, and that could cause an exception. If that is the case, we simply retry it.
    while (gen_attempts > 0) {
        try {
            // We flush the current map, it could contain pieces of old faulty generations.
            map_tree_time_.clear();
            // Then we start the generation
            for (auto itEntry = std::filesystem::recursive_directory_iterator(path);
                 itEntry != std::filesystem::recursive_directory_iterator();
                 ++itEntry) {
                // We take the current element path, make it relative to the path specified and then we make it
                // in a cross platform format (cross_platform_relative_element_path = cross_platform_rep)
                auto element_path = itEntry->path();
                std::filesystem::path relative_element_path = element_path.lexically_relative(path);
                std::string cross_platform_rep = relative_element_path.generic_string();

                // If is a directory we don't insert the tuple path-lmt. The list will only contain files.
                if (std::filesystem::is_directory(element_path)) {
                    continue;
                }

                //Read the lmt (last modified time) of the given path
                Database db;
                std::string time_str = db.getTimeFromPath(folder_name_string, cross_platform_rep, this->serverPath);

                //Convert the time from string to unsigned long
                unsigned long mod_time = std::stoul(time_str, nullptr, 0);

                //Insert into the map the couple path-lmt
                map_tree_time_.insert({cross_platform_rep, mod_time});

            }
            return;
        } catch(std::exception& e){
            gen_attempts--;
            std::cerr << "Generation of tree failed at attempt " << 10 - gen_attempts << std::endl;
            continue;
        }
    }

}

/**
 * Generate the list of path of the file available in the user server folder
 * @return list of path separated by '/n'
 */
std::string TreeT::genTree(){
    std::string tree;

    for(const auto& element : map_tree_time_) {
        tree.append(element.first + "\n");
    }

    return tree;
}

/**
 * Generate the list of times of the file available in the user server folder
 * @return list of time separated by '/n'
 */
std::string TreeT::genTimes(){
    std::string times;

    for(const auto& element : map_tree_time_) {
        times.append(std::to_string(element.second) + "\n");
    }

    return times;
}