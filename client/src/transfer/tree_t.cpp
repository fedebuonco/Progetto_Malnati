//
// Created by fede on 10/26/20.
//

#include <sstream>
#include "tree_t.h"
#include <stdlib.h>     /* strtoul */

/// Starting from the ordered string sent by the server, it computes A mapping between the files present in te server and their
/// Hashes
/// \param tree Directory tree if the server
/// \param time timw of the files present in the server following the same order of the tree
TreeT::TreeT(const std::string& tree, const std::string& time) {
    tree_ = tree;
    //Now we fill the map using the two string we got
    std::istringstream stream_tree{tree};
    std::istringstream stream_time{time};
    std::string filename;
    std::string file_time;
    while ((std::getline(stream_tree, filename)) && (std::getline(stream_time, file_time))) {
        time_.insert(std::pair<std::string ,unsigned long>(filename, std::stoul (file_time,nullptr,0)));
    }
}
