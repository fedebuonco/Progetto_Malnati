//
// Created by fede on 10/26/20.
//

#include "patch.h"

Patch::Patch(std::vector<std::string> add, std::vector<std::string> rem, std::vector<std::string> common) {
    added_ = add;
    removed_ = rem;
    common_ = common;
}

