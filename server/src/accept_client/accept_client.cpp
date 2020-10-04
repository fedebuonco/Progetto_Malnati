//
// Created by fede on 9/21/20.
//

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio.hpp>
#include "service.h"
#include "accept_client.h"

/* old
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
 */

AcceptClient::AcceptClient(asio::io_service& ios, unsigned short port_num) :
        ios_(ios),
        acceptor_(ios_,boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(),port_num))
{
    acceptor_.listen();
}

/// Creates an active socket and accepts an incoming connection by the client thus linking them and creating a Service class
/// that will take care of the handling
void AcceptClient::SpawnSession() {
    std::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(ios_));
    acceptor_.accept(*sock.get());
    //TODO what is the following syntax for? Creates a new session and also calls immediately a method?
    (new Service)->ReadRequest(sock);
}

