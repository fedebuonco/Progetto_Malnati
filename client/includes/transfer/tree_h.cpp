//
// Created by fede on 10/26/20.
//

#include <sstream>
#include "tree_h.h"

/// Starting from the ordered string sent by the server, it computes A mapping between the files present in te server and their
/// Hashes
/// \param tree Directory tree if the server
/// \param hash Hashes of the files present in the server following the same order of the tree
TreeH::TreeH(std::string tree, std::string hash) {
    tree_ = tree;
    //Now we fill the map using the two string we got
    std::istringstream stream_tree{tree};
    std::istringstream stream_hash{hash};
    std::string filename;
    std::string file_hash;
    while ((std::getline(stream_tree, filename)) && (std::getline(stream_hash, file_hash))) {
        hash_.insert( std::pair<std::string ,std::string>(filename,file_hash) );
    }
}
