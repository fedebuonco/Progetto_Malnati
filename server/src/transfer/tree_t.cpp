#include <sstream>
#include "tree_t.h"
#include <stdlib.h>     /* strtoul */
#include <vector>
#include <algorithm>
#include <sys/stat.h>

/// Starting from the ordered string sent by the server, it computes A mapping between the files present in te server and their
/// Hashes
/// \param tree Directory tree if the server
/// \param time timw of the files present in the server following the same order of the tree
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

TreeT::TreeT(const std::filesystem::path& path){

    this->folder_path_ = path;

    for(auto itEntry = std::filesystem::recursive_directory_iterator(path);
        itEntry != std::filesystem::recursive_directory_iterator();
        ++itEntry )
    {
        // We take the current element path, make it relative to the path specified and then we make it
        // in a cross platform format (cross_platform_relative_element_path = cross_platform_rep)
        auto element_path = itEntry->path();
        std::filesystem::path relative_element_path = element_path.lexically_relative(path);
        std::string cross_platform_rep = relative_element_path.generic_string();
        // We also add the "/" if it is a direcotry in order t diff it from non extension files.
        if (std::filesystem::is_directory(element_path))
            cross_platform_rep += "/";

        // // // // // // // // //
        // TODO retrieve the last modified time from the db and then uncomment this part and delete the otehr call in the block
        //
        // I assume that i have the std::string time_str;
        // unsigned long mod_time = std::stoul(time_str, nullptr, 0);
        // map_tree_time_.insert({cross_platform_rep, mod_time});
        //
        //
        //
        map_tree_time_.insert({cross_platform_rep, 1});
        // // // // // // // // //
    }
}

std::string TreeT::genTree(){
    std::string tree;
    for(const auto& element : map_tree_time_)
    {
        tree.append(element.first + "\n");
    }
    return tree;
}

std::string TreeT::genTimes(){
    std::string times;
    for(const auto& element : map_tree_time_)
    {
        times.append(std::to_string(element.second) + "\n");
    }
    return times;
}