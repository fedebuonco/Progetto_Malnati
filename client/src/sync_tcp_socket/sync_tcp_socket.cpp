//
// Created by fede on 9/21/20.
//

#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio.hpp>
#include <authentication.h>
#include "sync_tcp_socket.h"


SyncTCPSocket::SyncTCPSocket(const std::string& raw_ip_add, unsigned short port_n) :
                                           sock_(ios_) ,
                                           ep_(boost::asio::ip::address::from_string(raw_ip_add), port_n) {

    sock_.open(ep_.protocol());
}
SyncTCPSocket::~SyncTCPSocket() {
    // Exit from the connection and
    // TODO tell the server that we are exiting
    // TODO see shutdown exceptions and manage
    std::cout << "Connection and Socket closing down... " <<std::endl ;
    sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    sock_.close();
}
void SyncTCPSocket::ConnectServer() {
    int retry_connection = 5;
    while (retry_connection) {
        try {
            std::cout << "Establishing connection to server " << ep_.address() <<":"<<ep_.port() <<std::endl ;
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

    std::cout << "Server connesso\n" << std::endl;
}

/// Will send username and password. After that it will shutdown the send part of the socket thus providing the server a way to
/// tell that the connection is over
bool SyncTCPSocket::Authenticate() {

    Credential credential_ = Authentication::get_Instance()->ReadCredential();

    std::string auth_buf = credential_.username_ + " " + credential_.password_ + "\n";

    //int dim = buf.size();
    //boost::asio::write(sock_, boost::asio::buffer( std::to_string(dim) ));

    boost::asio::write(sock_, boost::asio::buffer(auth_buf));

    std::cout << "DEBUG: Sent  " << auth_buf << std::endl;
    //TODO we can do this only if we decide that syncTCPSocket is just for authentication (could be right could be wrong)
    sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

    //Now we use an extensible buffer for the uknwow size response
    boost::asio::streambuf response_buf;

    //Now let's wait the response using the same technique in the server ( we will shut down the sending part on the server)
    //prompting an eof error
    boost::system::error_code ec;
    boost::asio::read(sock_, response_buf, ec);
    if (ec == boost::asio::error::eof) {
        //We handle the response parsing a string and then printing it.
        std::istream is(&response_buf);
        std::string response_str;
        is >> response_str;
        std::cout << "Ho letto  " << response_str << std::endl;

        if(response_str=="1"){
            return true;
        } else
            return false;

    } else {
        //TODO handle qualcosa??
    }




}

