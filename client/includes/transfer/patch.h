//
// Created by fede on 10/26/20.
//

#ifndef CLIENT_PATCH_H
#define CLIENT_PATCH_H


#include <vector>
#include <string>

class Patch {
public:
    Patch(std::vector<std::string> vector, std::vector<std::string> vector1, std::vector<std::string> vector2);

    std::vector<std::string> added_;
    std::vector<std::string> removed_;
    std::vector<std::string> common_;

    void PrettyPrint();
};


#endif //CLIENT_PATCH_H
