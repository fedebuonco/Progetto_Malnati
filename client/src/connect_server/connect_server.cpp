//
// Created by fede on 9/21/20.
//

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio.hpp>
#include "connect_server.h"

ConnectServer::ConnectServer() {

    namespace pt = boost::property_tree;

    //This is the tree root; inside there is the username and password (if the app is config)
    pt::ptree  root;


    //Read the file and put the content inside root
    pt::read_json("../config_file/connection.json", root);

    auto raw_ip_address = root.get<std::string>("ip");
    auto port_num = root.get<unsigned short>("port");

    try {

        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(raw_ip_address), port_num);

        boost::asio::io_service ios;

        boost::asio::ip::tcp::socket sock(ios, ep.protocol());

        sock.connect(ep);
    }
    catch (boost::system::system_error &e) {
        //TODO
    }

    std::cout << "Connesso" << std::endl;
}
