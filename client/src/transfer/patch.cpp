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

/// We populate the to_be_sent_vector and the to_be_eliminated_vector
/// \param client_treet
/// \param server_treet
Patch::Patch(TreeT client_treet, TreeT server_treet){

    std::set<std::string> set_client;
    for(const auto& item : client_treet.map_tree_time_) {
        set_client.insert(item.first);
    }

    std::set<std::string> set_server;
    for(const auto& item : server_treet.map_tree_time_) {
        set_server.insert(item.first);
    }

    // Now I have the two sets I can compute set_difference(set_client,set_server)  client - server
    set_difference(set_client.begin(), set_client.end(), set_server.begin(), set_server.end(), inserter(added_, added_.end()));
    // Now I have the two sets I can compute set_difference(,set_server, set_client)  server - client
    set_difference(set_server.begin(), set_server.end(), set_client.begin(), set_client.end(), inserter(removed_, removed_.end()));
    // Now we can find the common files
    set_intersection(set_client.begin(), set_client.end(), set_server.begin(), set_server.end(), inserter(common_, common_.end()));

    // We gen the to_be_sent
    std::set_difference(begin(client_treet.map_tree_time_), end(client_treet.map_tree_time_),
                        begin(server_treet.map_tree_time_), end(server_treet.map_tree_time_),
                        std::back_inserter(to_be_sent_vector));


    // We gen the to_be_eliminated
    std::set_difference(begin(server_treet.map_tree_time_), end(server_treet.map_tree_time_),
                        begin(client_treet.map_tree_time_), end(client_treet.map_tree_time_),
                        std::back_inserter(to_be_elim_vector));
}

/// Takes the db files where we store the status and uses it in order to identify the file that we must dispatch.
/// \param db_path
/// \return int number of dispatched file.
int Patch::Dispatch(const std::filesystem::path db_path, const std::filesystem::path folder_watched){
    // TODO for each file in the to be sent look the status and if is it 0 insert it in the queue.
    int counter = 0;
    DatabaseConnection db(db_path, folder_watched);
    // Here we retrieve the metadata that is common to every filesipper: endpoint and usernmae
    // we also add 10 to the port.
    Credential credential = Authentication::get_Instance()->ReadCredential();
    RawEndpoint raw_endpoint = Config::get_Instance()->ReadRawEndpoint();
    raw_endpoint.port_num += 10;

    for ( auto element : to_be_sent_vector){
        if (db.ChangeStatusToSending(element.first)){ // THis return true only if the current status is "NEW" and changes it to "SENDING"
            // We retrieve the needed metadata for the file: hash and lmt
            std::string file_hash;
            std::string file_lmt;
            db.GetMetadata(element.first, file_hash, file_lmt);

            try {
                std::filesystem::path f = folder_watched / element.first;
                // TODO craeazione filessiper nello heap.
                // TODO Insert nella queue.
                auto fs =  new FileSipper(raw_endpoint,credential.username_, f, element.first, file_hash, file_lmt);
                fs->Send();
                counter++;

            } catch(std::exception& e)
            {
                std::cerr << "Erporre" << e.what() << std::endl;
                std::exit(123213);
            }

        }
    }
    return counter;
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
    pretty.append(":::::::: Files that will be Sent or are in sending (New files or newer files) - Last Modified Time ::::::::\n" );
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
    std::cout << ":::::::: Files that will be Sent or are in sending (New files or newer files) - Last Modified Time ::::::::" << std::endl;
    for (auto file : to_be_sent_vector){
        std::cout << file.first + " - " << file.second << std::endl;
    }
    std::cout << ":::::::: Recap : new files (" << added_.size() << ")   ::::::::" << std::endl;
    std::cout << ":::::::: Recap : removed files (" << removed_.size() << ")   ::::::::" << std::endl;
    std::cout << ":::::::: Recap : common files (" << common_.size() << ")   ::::::::" << std::endl;
    std::cout << ":::::::: Recap : Files that will be sent or are in sending (" << to_be_sent_vector.size() << ")   ::::::::" << std::endl;


    return pretty;
}


