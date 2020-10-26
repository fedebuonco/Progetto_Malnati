//
// Created by fede on 10/12/20.
//

#include <sync_tcp_socket.h>
#include <control_message.h>
#include <authentication.h>
#include "../../includes/client/client.h"
#include <boost/asio.hpp>
#include <filesystem>
#include <iostream>
#include <set>
#include <tree_h.h>

/// Construct a Client.
/// \param re Endpoint to connect to.
Client::Client(RawEndpoint re) {
    this->server_re_ = re;
    //TODO Istanziare una table dei socket attivi? BOh
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

/// Request current tree of the cloud dir stored in the server. Handles the result by starting the diff
/// computation.
TreeH Client::RequestTree() {
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

    //TODO Uncomment this as soon as the server will send the hashes
    //std::string hash = response_message.GetElement("Hash");
    //TreeH result{tree,hash};
    TreeH result{tree,""};
    return result;
}

/// Generete the diff between two string containing the tree of the client and the server (one file/dir for each line).
/// \param c_tree String of files/dir (one for each line) contained in the client folder
/// \param s_tree String of files/dir (one for each line) contained in the server folder
/// \return a string containing the diff in the format decided beforehand.
std::string Client::GenerateDiff(std::string c_tree, std::string s_tree) {

    //Here we take the two string containng the paths and we create a set of path for each
    std::set<std::string> set_client;
    std::set<std::string> set_server;
    std::vector<std::string> diff;

    // set_client
    std::istringstream ss_c(c_tree);
    std::string line_c;
    while (getline(ss_c, line_c)) {
        set_client.insert(line_c);
    }

    //set_server
    std::istringstream ss_s(s_tree);
    std::string line_s;
    while (getline(ss_s, line_s)) {
        set_server.insert(line_s);
    }

    //Now i have the two sets I can compute set_difference(set_client,set_server)  client - server
    set_difference(set_client.begin(), set_client.end(), set_server.begin(), set_server.end(), inserter(diff, diff.end()));
    std::string diff_str;

    for(const auto& value: diff) {
        diff_str.append("+ " + value+"\n");
    }

    //let's clear the vector diff and reuse it
    diff.clear();
    //Now i can make server-client
    set_difference(set_server.begin(), set_server.end(), set_client.begin(), set_client.end(), inserter(diff, diff.end()));
    for(const auto& value: diff) {
        diff_str.append("- " + value+"\n");
    }

    //let's clear the vector diff and reuse it
    diff.clear();
    //Now we print the common files
    set_intersection(set_server.begin(), set_server.end(), set_client.begin(), set_client.end(), inserter(diff, diff.end()));
    for(const auto& value: diff) {
        diff_str.append("= " + value+"\n");
    }

    return diff_str;
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