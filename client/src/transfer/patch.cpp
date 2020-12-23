#include <iostream>
#include <filesystem>
#include <algorithm>
#include <config.h>
#include <set>
#include "patch.h"

/// We populate the to_be_sent_vector an the to be elimnated
/// \param client_treet
/// \param server_treet
Patch::Patch(TreeT client_treet, TreeT server_treet){

    std::set<std::string> set_client;
    for(auto item : client_treet.map_tree_time_) {
        set_client.insert(item.first);
    }

    std::set<std::string> set_server;
    for(auto item : server_treet.map_tree_time_) {
        set_server.insert(item.first);
    }

    //Now i have the two sets I can compute set_difference(set_client,set_server)  client - server
    set_difference(set_client.begin(), set_client.end(), set_server.begin(), set_server.end(), inserter(added_, added_.end()));
    //Now i have the two sets I can compute set_difference(,set_server, set_client)  server - client
    set_difference(set_server.begin(), set_server.end(), set_client.begin(), set_client.end(), inserter(removed_, removed_.end()));
    //Now we can find the common files
    set_intersection(set_client.begin(), set_client.end(), set_server.begin(), set_server.end(), inserter(common_, common_.end()));

    //here we gen the to be sent
    std::set_difference(begin(client_treet.map_tree_time_), end(client_treet.map_tree_time_),
                        begin(server_treet.map_tree_time_), end(server_treet.map_tree_time_),
                        std::back_inserter(to_be_sent_vector));


    //here we gen the to be elim
    std::set_difference(begin(server_treet.map_tree_time_), end(server_treet.map_tree_time_),
                        begin(client_treet.map_tree_time_), end(client_treet.map_tree_time_),
                        std::back_inserter(to_be_elim_vector));



}

/// Pretty Prints the changes contained in the patch
/// \return A string summing up the current client-server file situation.
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
    pretty.append( ":::::::: Files that will be deleted on the server ( Because older or deleted ) - Last Modified Time ::::::::\n");
    for (auto file : to_be_elim_vector){
        pretty.append(file.first + " - ");
        pretty.append(std::to_string(file.second) + "\n");
    }
    pretty.append(":::::::: Files that will be Sent (New files or newer files) - Last Modified Time ::::::::\n" );
    for (auto file : to_be_sent_vector ){
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

    std::cout << ":::::::: Files that will be deleted on the server ( Because older or deleted ) - Last Modified Time ::::::::" << std::endl;
    for (auto file : to_be_elim_vector){
        std::cout << file.first + " - " << file.second << std::endl;
    }
    std::cout << ":::::::: Files that will be Sent (New files or newer files) - Last Modified Time ::::::::" << std::endl;
    for (auto file : to_be_sent_vector){
        std::cout << file.first + " - " << file.second << std::endl;
    }
    std::cout << ":::::::: Recap : new files (" << added_.size() << ")   ::::::::" << std::endl;
    std::cout << ":::::::: Recap : removed files (" << removed_.size() << ")   ::::::::" << std::endl;
    std::cout << ":::::::: Recap : common files (" << common_.size() << ")   ::::::::" << std::endl;
    std::cout << ":::::::: Recap : Files that will be sent (" << to_be_sent_vector.size() << ")   ::::::::" << std::endl;


    return pretty;
}


