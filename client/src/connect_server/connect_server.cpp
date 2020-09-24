//
// Created by fede on 9/21/20.
//

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio.hpp>
#include "connect_server.h"


ConnectServer::ConnectServer(const std::string& raw_ip_add,unsigned short port_n) :
                                           sock_(ios_) ,
                                           ep_(boost::asio::ip::address::from_string(raw_ip_add), port_n) {
    int retry_connection = 5;
    while (retry_connection) {
        try {
            std::cout << "Establishing connection to server " << raw_ip_add <<std::endl ;
            sock_.open(ep_.protocol());
            sock_.connect(ep_);
            break; // Usciamo dal while perchè connessione avvenuta con successo
        }
            catch (boost::system::system_error &e) {
            retry_connection--;
            if(retry_connection == 0){
                //TODO vedere std::exit e cose varie
                std::cerr << "Connection Problem - Server not responding or wrong IP address/Port (" << e.code().value() << ")" <<std::endl ;
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

ConnectServer::~ConnectServer() {
    // Exit from the connection and
    //TODO tell the server that we are exiting
    //TODO see shutdown exceptions and manage
    sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    sock_.close();
}
