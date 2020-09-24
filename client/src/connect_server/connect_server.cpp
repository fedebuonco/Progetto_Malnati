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
    int retry_connection = 5;

    //This is the tree root; inside there is the username and password (if the app is config)
    pt::ptree  root;

    // TODO gestire errori nella lettura del json
    //Read the file and put the content inside root
    pt::read_json("../config_file/connection.json", root);

    auto raw_ip_address = root.get<std::string>("ip");
    auto port_num = root.get<unsigned short>("port");

    while (retry_connection) {
        try {
            std::cout << "Establishing connection to server " << raw_ip_address <<std::endl ;
            boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(raw_ip_address), port_num);
            boost::asio::io_service ios;
            boost::asio::ip::tcp::socket sock(ios, ep.protocol());

            sock.connect(ep);
            break; // Usciamo dal while perchè connessione avvenuta con successo
        }
            catch (boost::system::system_error &e) {
            retry_connection--;
            if(retry_connection == 0){
                //TODO vedere std::exit e cose varie
                std::cerr << "Connection Problem - Server not responding or wrong IP address/Port (" << e.code() << ")" <<std::endl ;
                std::exit(1002);
            }
        }
        catch (std::exception& e) {
            //TODO vedere std::exit e cose varie
            std::cerr << "Generic Error during server connection" << std::endl ;
            std::exit(1001);
        }
    }

    std::cout << "Connesso" << std::endl;

}
