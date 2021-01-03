//
// Created by fede on 9/21/20.
//

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio.hpp>
#include "service.h"
#include "accept_client.h"
#include <filesystem>

AcceptClient::AcceptClient(asio::io_service& ios, unsigned short port_num,std::filesystem::path serverPath) :
        ios_(ios),
        acceptor_(ios_,boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(),port_num)),
        serverPath(serverPath)
{
    acceptor_.listen();
}

/// Creates an active socket and accepts an incoming connection by the client thus linking them and creating a Service class
/// that will take care of the handling. This service class will obtain the pointer to the socket here created.
/// The accept is blocking and has a own list of incoming connections.
void AcceptClient::SpawnSession() {

    std::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(ios_));

    acceptor_.accept(*sock.get());
    //TODO what is the following syntax for? Creates a new session and also calls immediately a method?
    (new Service)->ReadRequest(sock, this->serverPath);

}


