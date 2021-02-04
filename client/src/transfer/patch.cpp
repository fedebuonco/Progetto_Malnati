#include <iostream>
#include <filesystem>
#include <algorithm>
#include <config.h>
#include <iterator>
#include <set>
#include <database.h>
#include <file_sipper.h>
#include <authentication.h>
#include "patch.h"
#include <sender.h>

/// We populate the to_be_sent_vector and the to_be_eliminated_vector
/// \param client_treet
/// \param server_treet

bool EndsWith (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

Patch::Patch(TreeT client_treet, TreeT server_treet){

    std::set<std::string> set_client;
    for(const auto& item : client_treet.map_tree_time_) {
        if (!EndsWith(item.first, "/"))
            set_client.insert(item.first);
    }

    // We remove the hash.db from the set, it will not be considered in the dispatch
    set_client.erase(".hash.db");
    // We also remove all the folders, we only compute diff on files
    // we can easily remove all direcotries if we just delete all the element with "/"


    std::set<std::string> set_server;
    for(const auto& item : server_treet.map_tree_time_) {
        if (!EndsWith(item.first, "/"))
        set_server.insert(item.first);
    }


    try {
        // Now I have the two sets I can compute set_difference(set_client,set_server)  client - server
        set_difference(set_client.begin(), set_client.end(), set_server.begin(), set_server.end(), inserter(added_, added_.end()));
        // Now I have the two sets I can compute set_difference(,set_server, set_client)  server - client
        set_difference(set_server.begin(), set_server.end(), set_client.begin(), set_client.end(), inserter(removed_, removed_.end()));
        // Now we can find the common files
        set_intersection(set_client.begin(), set_client.end(), set_server.begin(), set_server.end(), inserter(common_, common_.end()));

    }catch(std::bad_alloc& badAlloc){
        std::cerr << " Patch Allocation error: " << badAlloc.what() <<std::endl;
        std::exit(EXIT_FAILURE);

    }

    for (auto item : server_treet.map_tree_time_){
        if (EndsWith(item.first, "/"))
            server_treet.map_tree_time_.erase(item.first);
    }

    try{
        std::set_difference(begin(client_treet.map_tree_time_), end(client_treet.map_tree_time_),
                            begin(server_treet.map_tree_time_), end(server_treet.map_tree_time_),
                            std::back_inserter(to_be_sent_vector));


        // We gen the to_be_eliminated
        std::set_difference(begin(server_treet.map_tree_time_), end(server_treet.map_tree_time_),
                            begin(client_treet.map_tree_time_), end(client_treet.map_tree_time_),
                            std::back_inserter(to_be_elim_vector));

    }catch(std::bad_alloc& badAlloc){
        std::cerr << " Patch Allocation error: " << badAlloc.what() <<std::endl;
        std::exit(EXIT_FAILURE);
    }

}

/// Takes the db files where we store the status and uses it in order to identify the file that we must dispatch.
/// \param db_path
/// \return int number of dispatched file.
int Patch::Dispatch(const std::filesystem::path db_path, const std::filesystem::path folder_watched){
    int counter = 0;
    DatabaseConnection db(db_path, folder_watched);
    // Here we retrieve the metadata that is common to every filesipper: endpoint and usernmae
    // we also add 10 to the port.
    Credential credential = Authentication::get_Instance()->ReadCredential();
    RawEndpoint raw_endpoint = Config::get_Instance()->ReadRawEndpoint();
    raw_endpoint.port_num += 10;

    for ( auto element : to_be_sent_vector){
        if (db.ChangeStatusToSending(element.first)){ // This return true only if the current status is "NEW" and changes it to "SENDING"
            // We retrieve the needed metadata for the file: hash and lmt
            std::string file_hash;
            std::string file_lmt;
            db.GetMetadata(element.first, file_hash, file_lmt);

            try {
                std::filesystem::path f = folder_watched / element.first;
                auto fs =  std::make_shared<FileSipper>(raw_endpoint, folder_watched , db_path ,credential.username_, f, element.first, file_hash, file_lmt);
                SharedQueue::get_Instance()->insert(fs);

                counter++;

            } catch(std::exception& e)
            {
                std::cerr << "Erorre" << e.what() << std::endl;
                std::exit(EXIT_FAILURE);
            }

        }
    }
    return counter;
}

/// Pretty Prints the changes contained in the patch
/// \return A string summing up the current client-server file situation.
std::string Patch::PrettyPrint(){
    int max_files_displayed = 3;
    std::string pretty;
    pretty.append(":::::::: Changes ::::::::\n");
    int cnt =0;
    for (auto file : added_){
        pretty.append("+ " + file +"\n");
        cnt++;
        if (cnt == max_files_displayed) {
            std::string other = std::to_string(added_.size() - 3);
            pretty.append("+ " + other + " Other files... \n");
            cnt = 0;
            break;
        }
    }
    for (auto file : removed_){
        pretty.append("- " + file +"\n");
        cnt++;
        if (cnt == max_files_displayed) {
            std::string other = std::to_string(removed_.size() - 3);
            pretty.append("- " + other + " Other files... \n");
            cnt = 0;
            break;
        }
    }
    for (auto file : common_){
        pretty.append("= " + file +"\n");
        cnt++;
        if (cnt == max_files_displayed) {
            std::string other = std::to_string(common_.size() - 3);
            pretty.append("= " + other + " Other files... \n");
            cnt = 0;
            break;
        }
    }
    pretty.append( ":::::::: Files that will be deleted or overwritten on the server ( Because older or deleted ) - Last Modified Time ::::::::\n");
    for (auto file : to_be_elim_vector){
        pretty.append(file.first + " - ");
        pretty.append(std::to_string(file.second) + "\n");
        cnt++;
        if (cnt == max_files_displayed) {
            std::string other = std::to_string(to_be_elim_vector.size() - 3);
            pretty.append("+ " + other + " Other files... \n");
            cnt = 0;
            break;
        }
    }
    pretty.append(":::::::: Files that will be Sent or are in sending (New files or newer files) - Last Modified Time ::::::::\n" );
    for (auto file : to_be_sent_vector ){
        pretty.append(file.first + " - ");
        pretty.append(std::to_string(file.second) + "\n");
        cnt++;
        if (cnt == max_files_displayed) {
            std::string other = std::to_string(to_be_sent_vector.size() - 3);
            pretty.append("+ " + other + " Other files... \n");
            cnt = 0;
            break;
        }
    }

    std::cout << ":::::::: Changes ::::::::" << std::endl;
    for (auto file : added_){
        std::cout <<"+ "<<file << std::endl;
        cnt++;
        if (cnt == max_files_displayed) {
            std::string other = std::to_string(added_.size() - 3);
            std::cout << "+ " + other + " Other files... \n" << std::endl;
            cnt = 0;
            break;
        }
    }
    for (auto file : removed_){
        std::cout <<"- " << file << std::endl;
        cnt++;
        if (cnt == max_files_displayed) {
            std::string other = std::to_string(removed_.size() - 3);
            std::cout << "- " + other + " Other files... \n" << std::endl;
            cnt = 0;
            break;
        }
    }
    for (auto file : common_){
        std::cout <<"= " << file << std::endl;
        cnt++;
        if (cnt == max_files_displayed) {
            std::string other = std::to_string(common_.size() - 3);
            std::cout << "= " + other + " Other files... \n" << std::endl;
            cnt = 0;
            break;
        }
    }

    std::cout << ":::::::: Files that will be deleted or overwritten on the server ( Because older or deleted ) - Last Modified Time ::::::::" << std::endl;
    for (auto file : to_be_elim_vector){
        std::cout << file.first + " - " << file.second << std::endl;
        cnt++;
        if (cnt == max_files_displayed) {
            std::string other = std::to_string(to_be_elim_vector.size() - 3);
            std::cout << "+ " + other + " Other files... \n" << std::endl;
            cnt = 0;
            break;
        }
    }
    std::cout << ":::::::: Files that will be Sent or are in sending (New files or newer files) - Last Modified Time ::::::::" << std::endl;
    for (auto file : to_be_sent_vector){
        std::cout << file.first + " - " << file.second << std::endl;
        cnt++;
        if (cnt == max_files_displayed) {
            std::string other = std::to_string(to_be_sent_vector.size() - 3);
            std::cout << "+ " + other + " Other files... \n" << std::endl;
            cnt = 0;
            break;
        }
    }
    std::cout << ":::::::: Recap : new files (" << added_.size() << ")   ::::::::" << std::endl;
    std::cout << ":::::::: Recap : removed files (" << removed_.size() << ")   ::::::::" << std::endl;
    std::cout << ":::::::: Recap : common files (" << common_.size() << ")   ::::::::" << std::endl;
    std::cout << ":::::::: Recap : Files that will be sent or are in sending (" << to_be_sent_vector.size() << ")   ::::::::" << std::endl;

    return pretty;
}


