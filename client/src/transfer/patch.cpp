//
// Created by fede on 10/26/20.
//

#include <iostream>
#include "patch.h"

Patch::Patch(std::vector<std::string> add, std::vector<std::string> rem, std::vector<std::string> common) {
    added_ = add;
    removed_ = rem;
    common_ = common;
}
void Patch::PrettyPrint(){
    for (auto file : added_){
        std::cout <<"+ "<<file << std::endl;
    }
    for (auto file : removed_){
        std::cout <<"- " << file << std::endl;
    }
    for (auto file : common_){
        std::cout <<"= " << file << std::endl;
    }

}