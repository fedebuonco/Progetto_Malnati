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
    std::map<std::string,unsigned long > time_;
    TreeT(const std::string& tree, const std::string& time);
};


#endif //CLIENT_TREE_T_H
