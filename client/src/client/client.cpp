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
    //TODO qualche eccezione da fare il catch?

    //SyncTCPSocket for auth using raw endpoint provided at construction.
    SyncTCPSocket tcpSocket(server_re_.raw_ip_address, server_re_.port_num);
    //socket will retry 5 times to connect
    tcpSocket.ConnectServer(5);

    //Try to authenticate and return true / false
    return tcpSocket.Authenticate();

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

    ControlMessage message_obj{2};
    //Adding Username and Hash Password
    message_obj.AddElement("Username", credential.username_);
    message_obj.AddElement("HashPassword:", credential.hash_password_);

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
        //TODO Alcune volte strane si blocca
        //qua se non ho ricevuto la chiusura del client
        if(DEBUG) std::cerr<<"DEBUG: NON ho ricevuto il segnale di chiusura del client";
        throw boost::system::system_error(ec);
    }

    //Read the response_buf using an iterator and store it in a string
    //TODO might be an easier method to do this
    std::string response_json((std::istreambuf_iterator<char>(&response_buf)), std::istreambuf_iterator<char>() );

    //Now we parsed the request and we use the json in order to create the corresponding ControlMessage
    ControlMessage response_message{response_json};
    if(DEBUG) std::cout << "Tree received successfully" << std::endl;

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
Patch Client::GeneratePatch(std::filesystem::path mon_folder, const std::string& client_t,const std::string& server_t) {
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
    Patch result{mon_folder, added,removed,common};
    return result;
}

/// This generate a directory tree following the tree command protocol available on linux
/// \param path Tree will be generated of this path
/// \return String of files/dir (one for each line) contained in the folder
std::string Client::GenerateTree(const std::filesystem::path& path) {


    std::vector<std::string> vector_result;
    std::string result;

    for(auto itEntry = std::filesystem::recursive_directory_iterator(path);
        itEntry != std::filesystem::recursive_directory_iterator();
        ++itEntry )
    {
        const auto filepath = itEntry->path();
        std::filesystem::path clean_filepath = filepath.lexically_relative(path);
        std::string file_str = clean_filepath.generic_string();
        if (std::filesystem::is_directory(filepath)) {
            vector_result.push_back(file_str + "/" + '\n');
        } else {
            vector_result.push_back(file_str+ '\n');
        }
    }

    //Here we sort the tree in alphabetic order to permit cross platform diff
    std::sort(vector_result.begin(), vector_result.end());
    for(auto clean_path : vector_result )
    {
        result.append(clean_path);
    }

    return result;
}

/// We create a map "Filename - last modified time"
/// \param patch
void Client::ProcessNew(Patch& patch) {
    for (auto file_path : patch.added_ ){
        //TODO THIS works only on linux find a windows solution _stat could be used

        //Here we have the file_path but it is formatted not for stat,
        //we must pass it to stat so we need to append it to the monitored folder
        std::filesystem::path complete_path = patch.monitored_folder_;
        complete_path /= file_path;
        if(DEBUG){
            std::cout << "Process New - " << complete_path.generic_string() << std::endl;
        }
        struct stat result;
        if(stat(complete_path.generic_string().c_str(), &result)==0){
            auto mod_time = result.st_mtime;
            std::pair<std::string, unsigned long int> element = std::make_pair (file_path,mod_time);
            patch.to_be_sent_map_.insert(element);

        }
    }
}

/// This function compare the files that are present both in server and client. It compares them
/// based on their last modified time.
/// \param patch
void Client::ProcessCommon(Patch& patch, TreeT server_treet){

    std::map<std::string, unsigned long> client_common_map;

    for (auto file_path : patch.common_ ) {

        //Here we have the file_path but it is formatted not for stat,
        //we must pass it to stat so we need to append it to the monitored folder
        std::filesystem::path complete_path = patch.monitored_folder_;
        complete_path /= file_path;

        //We generate the client_common_map
        struct stat result;
        if (stat(complete_path.generic_string().c_str(), &result) == 0) {
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
                //Thus, we add them to the to_be_sent_map, will be used in the sendPatch.
                patch.to_be_sent_map_.insert(pair);
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
    delete_message.AddElement("Password:",credential_.hash_password_ );
    //We add the delete string
    delete_message.AddElement("To_be_deleted",update.to_be_deleted_);
    //And sending it formatted in JSON language
    boost::asio::write(tcpSocket.sock_, boost::asio::buffer(delete_message.ToJSON()));
    // we sent the Auth message, we will shutdown in order to tell the server that we sent all
    tcpSocket.sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

    // Now we can focus on new and common
    //TODO Here send the files asyncronulsy

}