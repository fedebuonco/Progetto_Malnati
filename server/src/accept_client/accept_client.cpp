//
// Created by fede on 9/21/20.
//

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio.hpp>
#include "accept_client.h"


AcceptClient::AcceptClient() {

    unsigned short port_num = 3333;

    try {

        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), port_num);

        boost::asio::io_service ios;

        boost::asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

        acceptor.bind(ep);

        acceptor.listen(BACKLOG_SIZE);

        boost::asio::ip::tcp::socket sock(ios);

        acceptor.accept();
    }
    catch (boost::system::system_error &e) {
            //TODO vedere std::exit e cose varie
            std::cerr << "Connection Problem - Server not responding or wrong IP address/Port (" << e.code() << ")" <<std::endl ;
    }
    catch (std::exception& e) {
        //TODO vedere std::exit e cose varie
        std::cerr << "Generic Error during server connection" << std::endl ;
    }


    std::cout << "Connesso " << std::endl;

}
