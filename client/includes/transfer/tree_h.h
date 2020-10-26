//
// Created by fede on 10/26/20.
//

#ifndef CLIENT_TREE_H_H
#define CLIENT_TREE_H_H


#include <string>
#include <map>

class TreeH {
public:
    std::string tree_;
    std::map<std::string,std::string> hash_;
    TreeH(std::string tree, std::string hash);
};


#endif //CLIENT_TREE_H_H
