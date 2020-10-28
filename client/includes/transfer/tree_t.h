//
// Created by fede on 10/26/20.
//

#ifndef CLIENT_TREE_T_H
#define CLIENT_TREE_T_H


#include <string>
#include <map>

class TreeT {
public:
    std::string tree_;
    std::map<std::string,std::string> time_;
    TreeT(std::string tree, std::string time);
};


#endif //CLIENT_TREE_T_H
