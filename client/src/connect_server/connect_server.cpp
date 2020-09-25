//
// Created by fede on 9/21/20.
//

#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio.hpp>
#include <authentication.h>
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

    std::cout << "Server connesso\n" << std::endl;

}

ConnectServer::~ConnectServer() {
    // Exit from the connection and
    // TODO tell the server that we are exiting
    // TODO see shutdown exceptions and manage
    std::cout << "Connection and Socket closing down... " <<std::endl ;
    sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    sock_.close();
}

bool ConnectServer::Authenticate() {

    Credential credential_ = Authentication::get_Instance()->ReadCredential();

    std::string buf = credential_.username_ + " " + credential_.password_ + "\n";



    int dim = buf.size();


    //boost::asio::write(sock_, boost::asio::buffer( std::to_string(dim) ));

    boost::asio::write(sock_, boost::asio::buffer(buf));

    std::cout << "Ho mandato " << buf << "di dim " << dim << std::endl;



    const unsigned char MESSAGE_SIZE = 1;

    char buf2[MESSAGE_SIZE];

    boost::asio::read(sock_, boost::asio::buffer(buf2, MESSAGE_SIZE));

    std::string read_ = std::string(buf2, MESSAGE_SIZE);

    std::cout << "Ho letto  " << read_ << std::endl;

    if(read_=="1"){

        return true;
    } else
    return false;
}
