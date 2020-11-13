#include <iostream>
#include <filesystem>
#include "patch.h"

/// This is the constructor for the Patch, before it get sent we have to process it. The 3 vectors are needed
/// in order generate the additional data struct taht will be serialized and sent.
/// \param add Contains the filenames of elements that are in the client and not in the server, will be sent
/// \param rem Contains the filneames of element that are in the server but not in the client, will be deleted
/// \param common Contains the filenames of element that are both in server and client, will be checked.
Patch::Patch(std::filesystem::path mon_folder, std::vector<std::string> add, std::vector<std::string> rem, std::vector<std::string> common) {
    added_ = add;
    removed_ = rem;
    common_ = common;
    monitored_folder_ = mon_folder;
}

/// Pretty Prints the changes contained in the patch
std::string Patch::PrettyPrint(){

    std::string pretty;
    pretty.append(":::::::: Changes ::::::::\n");
    for (auto file : added_){
        pretty.append("+ " + file +"\n");
    }
    for (auto file : removed_){
        pretty.append("- " + file +"\n");
    }
    for (auto file : common_){
        pretty.append("= " + file +"\n");
    }
    pretty.append( ":::::::: Deleted Files ::::::::\n");
    pretty.append(to_be_deleted_);
    pretty.append(":::::::: Files that will be Sent - Last Modified Time ::::::::\n" );
    for (auto file : to_be_sent_map_){
        std::cout << file.first + " - " << file.second << std::endl;
        pretty.append(file.first + " - ");
        pretty.append(std::to_string(file.second) + "\n");
    }

    std::cout << ":::::::: Changes ::::::::" << std::endl;
    for (auto file : added_){
        std::cout <<"+ "<<file << std::endl;
    }
    for (auto file : removed_){
        std::cout <<"- " << file << std::endl;
    }
    for (auto file : common_){
        std::cout <<"= " << file << std::endl;
    }

    std::cout << ":::::::: Deleted Files ::::::::" << std::endl;
    std::cout << to_be_deleted_ << std::endl;
    std::cout << ":::::::: Files that will be Sent - Last Modified Time ::::::::" << std::endl;
    for (auto file : to_be_sent_map_){
        std::cout << file.first + " - " << file.second << std::endl;
    }


    return pretty;
}