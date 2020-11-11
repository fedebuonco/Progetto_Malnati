//
// Created by fede on 10/12/20.
//

//CROSS platform get last modified time
//#include <sys/types.h>
//#include <sys/stat.h>
//#ifndef _WIN32
//#include <unistd.h>
//#endif
//#ifdef _WIN32
//#define stat _stat
//#endif

#include <sync_tcp_socket.h>
#include <control_message.h>
#include <authentication.h>
#include "../../includes/client/client.h"
#include <boost/asio.hpp>
#include <filesystem>
#include <iostream>
#include <set>
#include <tree_t.h>
#include <patch.h>
#include <chrono>

/// Construct a Client.
/// \param re Endpoint to connect to.
Client::Client(RawEndpoint re) {
    this->server_re_ = re;
}

/// Authenticates the client by creating a syncTCPSocket and calling it's auth method.
/// \return
bool Client::Auth() {
    //TODO valutare se mettere try catch

    //SyncTCPSocket for auth using raw endpoint provided at construction.
    SyncTCPSocket tcpSocket(server_re_.raw_ip_address, server_re_.port_num);
    //socket will retry 5 times to connect
    tcpSocket.ConnectServer(5);

    if(! Config::get_Instance()->isConfig() ){
        Config::get_Instance()->startConfig();
    }
    //TODO Lanciare detro isConfig e startConfig delle eccezioni di una classe
    // da fare e poi catcharle nel main per terminare il programma.
    // Es. No indirizzo ip per connessione server

    //Auth loop, while the password is wrong asks for a new identity. Will exit as soon as
    //data inserted is verified correctly.
    while( !tcpSocket.Authenticate() ){
        Config::get_Instance()->startConfig();
    }

    return true;

}

/// Request current tree and times of the cloud dir stored in the server. Handles the result by starting the diff
/// computation.
TreeT Client::RequestTree() {
    //SyncTCPSocket for request
    Credential credential = Authentication::get_Instance()->ReadCredential();
    SyncTCPSocket tcpSocket(server_re_.raw_ip_address, server_re_.port_num);
    //socket will retry 5 times to connect
    tcpSocket.ConnectServer(5);

    //Here we create the ControlMessage for TreeRequest ( Type = 2 )
    //TODO this will be key not the actual username and pass
    ControlMessage message_obj{2};
    //Adding User and Password
    //TODO Change this after we decide to add keys
    message_obj.AddElement("Username",credential.username_);
    message_obj.AddElement("Password:",credential.password_);
    //And sending it formatted in JSON language
    boost::asio::write(tcpSocket.sock_, boost::asio::buffer(message_obj.ToJSON()));
    // we sent the tree req message, we will shutdown in order to tell the server that we sent all
    tcpSocket.sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

    //Here is the server tree
    boost::asio::streambuf response_buf;
    boost::system::error_code ec;
    boost::asio::read(tcpSocket.sock_, response_buf, ec);
    // This checks if the client has finished writing
    if (ec != boost::asio::error::eof){
        //qua se non ho ricevuto la chiusura del client
        std::cout<<"DEBUG: NON ho ricevuto il segnale di chiusura del client";
        throw boost::system::system_error(ec);
    }
    //Read the response_buf using an iterator and store it in a string
    //TODO might be an easier method to do this
    std::string response_json((std::istreambuf_iterator<char>(&response_buf)), std::istreambuf_iterator<char>() );

    //Now we parsed the request and we use the json in order to create the corresponding ControlMessage
    ControlMessage response_message{response_json};
    //std::cout << "Tree recived successfully" << std::endl;
    //We get the tree
    std::string tree = response_message.GetElement("Tree");

    //TODO Uncomment this as soon as the server will send the times
    //std::string time = response_message.GetElement("Time");
    //TreeT result{tree,time};
    TreeT result{tree, ""};
    return result;
}

/// Generete the diff between two string containing the tree of the client and the server (one file/dir for each line).
/// \param client_t String of files/dir (one for each line) contained in the client folder
/// \param server_t String of files/dir (one for each line) contained in the server folder
/// \return a string containing the diff in the format decided beforehand.
Patch Client::GeneratePatch(const std::string& client_t,const std::string& server_t) {
    //Here we take the two string containng the paths and we create a set of path for each
    std::set<std::string> set_client;
    std::set<std::string> set_server;
    std::vector<std::string> diff;

    // set_client
    std::istringstream ss_c(client_t);
    std::string line_c;
    while (getline(ss_c, line_c)) {
        set_client.insert(line_c);
    }

    //set_server
    std::istringstream ss_s(server_t);
    std::string line_s;
    while (getline(ss_s, line_s)) {
        set_server.insert(line_s);
    }

    //Now i have the two sets I can compute set_difference(set_client,set_server)  client - server
    set_difference(set_client.begin(), set_client.end(), set_server.begin(), set_server.end(), inserter(diff, diff.end()));
    std::string diff_str;

    std::vector<std::string> added;
    for(const auto& value: diff) {
        added.push_back(value);
    }

    //let's clear the vector diff and reuse it
    diff.clear();
    //Now i can make server-client
    std::vector<std::string> removed;
    set_difference(set_server.begin(), set_server.end(), set_client.begin(), set_client.end(), inserter(diff, diff.end()));
    for(const auto& value: diff) {
        removed.push_back(value);
    }

    //let's clear the vector diff and reuse it
    diff.clear();
    //Now we print the common files
    std::vector<std::string> common;
    set_intersection(set_server.begin(), set_server.end(), set_client.begin(), set_client.end(), inserter(diff, diff.end()));
    for(const auto& value: diff) {
        common.push_back(value);
    }

    //We can now create the patch object
    Patch result{added,removed,common};
    return result;
}

/// This generate a directory tree following the tree command protocol available on linux
/// \param path Tree will be generated of this path
/// \return String of files/dir (one for each line) contained in the folder
std::string Client::GenerateTree(const std::filesystem::path& path) {

    std::string result;
    for(auto itEntry = std::filesystem::recursive_directory_iterator(path);
        itEntry != std::filesystem::recursive_directory_iterator();
        ++itEntry ) {
        const auto filenameStr = itEntry->path().string();
        result.append(filenameStr + '\n');
    }
    return result;
}

/// We create a map "Filename - last modified time"
/// \param patch
void Client::ProcessNew(Patch& patch) {
    for (auto file_path : patch.added_ ){
        //TODO THIS works only on linux find a windows solution _stat could be used


        struct stat result;
        if(stat(file_path.c_str(), &result)==0){
            auto mod_time = result.st_mtime;
            std::pair<std::string, unsigned long int> element = std::make_pair (file_path,mod_time);
            patch.to_be_added_map_.insert(element);

        }




    }
}

/// This function compare the files that are present both in server and client. It compares them
/// based on their last modified time.
/// \param patch
void Client::ProcessCommon(Patch& patch, TreeT server_treet){

    std::map<std::string, unsigned long> client_common_map;

    for (auto file_path : patch.common_ ) {
        //TODO THIS works only on linux find a windows solution _stat could be used

        //We generate the client_common_map
        struct stat result;
        if (stat(file_path.c_str(), &result) == 0) {
            auto mod_time = result.st_mtime;
            std::pair<std::string, unsigned long> element = std::make_pair(file_path, mod_time);
            client_common_map.insert(element);
        }
    }

    if(DEBUG)
        std::cout << ":::::::: Conflicts ::::::::" <<std::endl;

    // Now we compare them with the map already present in the patch, that was sent by the server
    for (auto const& pair : client_common_map) {
        if(server_treet.time_[pair.first] < pair.second ){
            //Here we enter if in the client the file is newer.
            if(DEBUG) {
                std::cout << "#### Found Conflict - " << pair.first << " is newer in the client" << std::endl;
                //TODO here we should deicede what to do with them
            }
        }
    }



}


/// This function generates a string and fill the to_be_deleted_ member of the patch
/// containing all the files/dir that should be removed on the server side.
/// It is a string because it will be easier to serialize it via json. Will be sent in the SendPatch alogn with the other
/// changes
/// \param patch Patch containing the filenames of the " to be removed" files
void Client::ProcessRemoved(Patch& patch) {
    for (auto file_path : patch.removed_ ){
        patch.to_be_deleted_.append(file_path + "\n");
        // debug
    }
}

/// Here we firstly send a ControlMessage that will tell what to delete in the server. After that we will start sending
/// the newer files.
/// \param update processed patch
void Client::SendPatch(Patch& update){
    //Sending delete ControlMessage
    //Creation of the Auth ControlMessage type = 3
    Credential credential_ = Authentication::get_Instance()->ReadCredential();
    SyncTCPSocket tcpSocket(server_re_.raw_ip_address, server_re_.port_num);
    ControlMessage delete_message{3};
    //Adding User and Password
    //TODO Change this after we decide to add keys
    delete_message.AddElement("Username",credential_.username_);
    delete_message.AddElement("Password:",credential_.password_);
    //We add the delete string
    delete_message.AddElement("To_be_deleted",update.to_be_deleted_);
    //And sending it formatted in JSON language
    boost::asio::write(tcpSocket.sock_, boost::asio::buffer(delete_message.ToJSON()));
    // we sent the Auth message, we will shutdown in order to tell the server that we sent all
    tcpSocket.sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

    // Now we can focus on new and common
    //TODO Here send the files asyncronulsy

    /*
     *
     */

}