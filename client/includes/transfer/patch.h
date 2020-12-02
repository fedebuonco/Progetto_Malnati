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

class Patch {
public:

    std::vector<std::string> added_;
    std::vector<std::string> removed_;
    std::vector<std::string> common_;
    std::vector<std::pair<std::string, unsigned  long>> to_be_sent_vector;
    std::vector<std::pair<std::string, unsigned  long>> to_be_elim_vector;

    Patch(TreeT client_treet, TreeT server_treet);
    std::string PrettyPrint();

};


#endif //CLIENT_PATCH_H
