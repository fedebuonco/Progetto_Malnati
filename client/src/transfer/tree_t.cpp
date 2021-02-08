#include <sstream>
#include "tree_t.h"
#include <algorithm>
#include <stdlib.h>
#include <iostream>
//Necesario per linux
#include <sys/stat.h>


/// Starting from the ordered string sent by the server, it computes a mapping between the files present in the server and their hashes
/// \param tree: Directory tree if the servern files (0)   ::::::::i
/// \param time: Time of the files present in the server following the same order of the tree
TreeT::TreeT(const std::string& tree, const std::string& time) {
    //Now we fill the map using the two string we got
    std::istringstream stream_tree{tree};
    std::istringstream stream_time{time};
    std::string filename;
    std::string file_time;


    try {
        /**std::stoul Exceptions:
         * std::invalid_argument if no conversion could be performed
         * std::out_of_range if the converted value would fall out of
         *                      the range of the result type or if the
         *                      underlying function (std::strtoul or
         *                      std::strtoull) sets errno to ERANGE.
         */
        while ((std::getline(stream_tree, filename))) {
            if (std::getline(stream_time, file_time)) {
                map_tree_time_.insert(
                        std::pair<std::string, unsigned long>(filename, std::stoul(file_time, nullptr, 0)));
            } else {
                map_tree_time_.insert(std::pair<std::string, unsigned long>(filename, 0));
            }
        }
    }catch(std::invalid_argument& ia){
        //Error in conversion std::stoul
        std::cerr << "Invalid argument creating tree: " << ia.what() << '\n';
        std::exit(EXIT_FAILURE);

    }catch(std::out_of_range& orr){
        //Error in conversion std::stoul
        std::cerr << "Out of range creating tree: " << orr.what() << '\n';
        std::exit(EXIT_FAILURE);

    }

}

/// Build and populate the TreeT starting from a path.
/// \param path Path of the directory that will be stored in the TreeT.
TreeT::TreeT(const std::filesystem::path& path){
    std::error_code ec;

    try{

        for(auto itEntry = std::filesystem::recursive_directory_iterator(path,ec);
            itEntry != std::filesystem::recursive_directory_iterator();
            ++itEntry )
        {
            if(ec){
                std::cerr << "Error Recursive directory (TreeT): " << ec << std::endl;
                std::exit(EXIT_FAILURE);
            }
            // We take the current element path, make it relative to the path specified and then we make it
            // in a cross platform format (cross_platform_relative_element_path = cross_platform_rep)
            auto element_path = itEntry->path();
            std::filesystem::path relative_element_path = element_path.lexically_relative(path);
            std::string cross_platform_rep = relative_element_path.generic_string();

            //We don't insert folder, so we go to the next for iteration
            if (std::filesystem::is_directory(element_path)) continue;

            // We don't insert the hash.db
            if (cross_platform_rep == ".hash.db")
                continue;

            //we now need to retrieve the last modified time.
            struct stat temp_stat;
            stat(element_path.generic_string().c_str(), &temp_stat);
            unsigned long mod_time = temp_stat.st_mtime;
            map_tree_time_.insert({cross_platform_rep, mod_time});
        }

    }
    catch (std::filesystem::filesystem_error &e) {              //TODO: Policy? @marco
        std::cerr << "Error Filesystem (TreeT): " << e.what() << std::endl;
        //std::exit(EXIT_FAILURE);
    }
    catch (std::exception &e) {
        std::cerr << "Error generic (TreeT) " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

