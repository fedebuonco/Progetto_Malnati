//
// Created by fede on 10/26/20.
//

#ifndef CLIENT_PATCH_H
#define CLIENT_PATCH_H


#include <vector>
#include <string>
#include <chrono>
#include <map>
#include "tree_t.h"

/// Wraps in a compact way all the differences between the client and server.
/// This difference is computed starting from the TreeT of the client and of the server.
/// Will be "applied" to the server.
class Patch {
public:

    /// This vector contains the path of all the elements(files or dirs) in the client with a position
    /// and name not present in the server.
    std::vector<std::string> added_;

    /// This vector contains the path of all the elements(files or dirs) in the server with a position
    /// and name not present in the client.
    std::vector<std::string> removed_;

    /// This vector contains the path of all the elements(files or dirs) in the server with both position
    /// and name equal in the client. Position and name are equal but last modified time could be different.
    std::vector<std::string> common_;

    /// This vector holds all the pairs element_path&last-modified-time of every element that will be sent
    /// to the server ( could be a new file or a file newer that one in the server).
    std::vector<std::pair<std::string, unsigned  long>> to_be_sent_vector;

    /// This vector holds all the pairs element_path&last-modified-time of every element that will be deleted
    /// (or replaced) in the server ( could be a deleted file or a file older that one in the client and that we are
    /// currently sending).
    std::vector<std::pair<std::string, unsigned  long>> to_be_elim_vector;

    Patch(TreeT client_treet, TreeT server_treet);
    std::string PrettyPrint();

};


#endif //CLIENT_PATCH_H
