//
// Created by fede on 9/21/20.
//

#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio.hpp>
#include <control_message.h>
#include "authentication.h"
#include "sync_tcp_socket.h"

/// Initialize socket, endpoint and parse the raw ip. After that opens the socket using the specified protocol, TCP.
/// \param raw_ip_add Parsed raw ip
/// \param port port connecting to
SyncTCPSocket::SyncTCPSocket(const std::string& raw_ip_add, unsigned short port_n) :
                                           sock_(ios_) ,
                                           ep_(boost::asio::ip::address::from_string(raw_ip_add), port_n) {

    sock_.open(ep_.protocol());
}


/// Shutdown both part(sending & reciving) and closes the socket giving back the resource to the system.
SyncTCPSocket::~SyncTCPSocket() {
    // Exit from the connection and
    // TODO tell the server that we are exiting
    // TODO see shutdown exceptions and manage
    std::cout << "RawEndpoint and Socket closing down... " <<std::endl ;
    //TODO fix error in linux where wew have a exception when we shutodwn a scoket like this
    sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    sock_.close();
}


/// Connect the socket to the server. Tries the connection for a specified number of times
/// \param n_tries Number of re tries done.
void SyncTCPSocket::ConnectServer(int n_tries) {
    while (n_tries) {
        try {
            //TODO Add a delay good for linux and windows otherwise useless
            std::cout << "Establishing connection to server " << ep_.address() <<":"<<ep_.port() <<std::endl ;
            sock_.connect(ep_);
            break; // Usciamo dal while perchè connessione avvenuta con successo
        }
        catch (boost::system::system_error &e) {
            n_tries--;
            if(n_tries == 0){
                //TODO vedere std::exit e cose varie
                //
                std::cerr << "RawEndpoint Problem - Server not responding or wrong IP address/Port (" << e.code().value() << ")" <<std::endl ;
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
/// tell that the connection is over. After the authenticate the socket is basically useless and needs to be shutdown completly.
/// \return True if Auth went ok and user has successfully logged in, False if not.
bool SyncTCPSocket::Authenticate() {

    Credential credential_ = Authentication::get_Instance()->ReadCredential();

    std::string auth_buf = credential_.username_ + " " + credential_.password_ + "\n";

    //Creation Of the JSON string
    ControlMessage message_obj{1,credential_.username_,credential_.password_,""};
    std::string message_json = message_obj.ToJSON();

    boost::asio::write(sock_, boost::asio::buffer(message_json));

    std::cout << "DEBUG: Sent \n " << message_json << std::endl;
    sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

    //Now we use an extensible buffer for the uknouwn size response
    boost::asio::streambuf response_buf;

    //Now let's wait the response using theAcceptClient same technique in the server
    //(we will shut down the sending part on the server)
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

