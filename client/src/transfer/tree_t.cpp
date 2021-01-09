#include <sstream>
#include "tree_t.h"
#include <stdlib.h>     /* strtoul */
#include <vector>
#include <algorithm>
#include <sys/stat.h>

// TODO we must add hash in the treet filename,<hash,time> .


/// Starting from the ordered string sent by the server, it computes a mapping between the files present in the server and their hashes
/// \param tree: Directory tree if the server
/// \param time: Time of the files present in the server following the same order of the tree
TreeT::TreeT(const std::string& tree, const std::string& time) {
    //Now we fill the map using the two string we got
    std::istringstream stream_tree{tree};
    std::istringstream stream_time{time};
    std::string filename;
    std::string file_time;

    while (std::getline(stream_tree, filename)) {

        if (std::getline(stream_time, file_time)) {
            map_tree_time_.insert(std::pair<std::string, unsigned long>(filename, std::stoul(file_time, nullptr, 0)));
        } else {
            map_tree_time_.insert(std::pair<std::string, unsigned long>(filename, 0));
        }
    }
}

/// Build and populate the TreeT starting from a path.
/// \param path Path of the directory that will be stored in the TreeT.
TreeT::TreeT(const std::filesystem::path& path){

    for(auto itEntry = std::filesystem::recursive_directory_iterator(path);
        itEntry != std::filesystem::recursive_directory_iterator();
        ++itEntry )
    {
        // We take the current element path, make it relative to the path specified and then we make it
        // in a cross platform format (cross_platform_relative_element_path = cross_platform_rep)
        auto element_path = itEntry->path();
        std::filesystem::path relative_element_path = element_path.lexically_relative(path);
        std::string cross_platform_rep = relative_element_path.generic_string();

        // We also add the "/" if it is a directory in order to differentiate it from non extension files.
        if (std::filesystem::is_directory(element_path))
            cross_platform_rep += "/";

        //we now need to retrieve the last modified time.
        struct stat temp_stat;
        stat(element_path.generic_string().c_str(), &temp_stat);
        unsigned long mod_time = temp_stat.st_mtime;
        map_tree_time_.insert({cross_platform_rep, mod_time});
    }
}

