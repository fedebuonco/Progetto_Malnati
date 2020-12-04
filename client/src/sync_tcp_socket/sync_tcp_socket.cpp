//
// Created by fede on 9/21/20.
//

#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/asio.hpp>
#include <control_message.h>
#include <config.h>
#include "authentication.h"
#include "sync_tcp_socket.h"

/// Initialize socket, endpoint and parse the raw ip. After that opens the socket using the specified protocol, TCP.
/// \param raw_ip_add Parsed raw ip
/// \param port_n port connecting to
SyncTCPSocket::SyncTCPSocket(const std::string& raw_ip_add, unsigned short port_n) :
                                           sock_(ios_) ,
                                           ep_(boost::asio::ip::address::from_string(raw_ip_add), port_n) {

    sock_.open(ep_.protocol());
}

/// Shutdown both part(sending & reciving) and closes the socket giving back the resource to the system.
SyncTCPSocket::~SyncTCPSocket() {
    boost::system::error_code ec;
    //TODO qua magicamente va ignorato l'errore GRAVISSIMO
    sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    // Here we should read from the socket, catch the error and closing the socket
    sock_.close();
}

/// Connect the socket to the server. Tries the connection for a specified number of times
/// \param n_tries Number of re tries done.
void SyncTCPSocket::ConnectServer(int n_tries) {
    while (n_tries) {
        try {

            if (DEBUG)  std::cout << "\nEstablishing connection to server " << ep_.address() <<":"<<ep_.port() <<std::endl ;

            sock_.connect(ep_);
            break; // Usciamo dal while perchè connessione avvenuta con successo
        }
        catch (boost::system::system_error &e) {
            n_tries--;

            //TODO CONTROLLARE BENE Add a delay good for linux and windows otherwise useless
            std::this_thread::sleep_for (std::chrono::seconds (1));
            //std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::nanoseconds(1) );

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

    // std::cout << "Server connesso\n" << std::endl;
}

/// Will send username and password. After that it will shutdown the send part of the socket thus providing the server a way to
/// tell that the connection is over. After the authenticate the socket is basically useless and needs to be shutdown completly.
/// \return True if Auth went ok and user has successfully logged in, False if not.
bool SyncTCPSocket::Authenticate() {

    Credential credential = Authentication::get_Instance()->ReadCredential();

    //Creation of the Auth ControlMessage type = 1
    ControlMessage auth_message{1};

    //Adding User and Password
    auth_message.AddElement("Username", credential.username_);
    auth_message.AddElement("HashPassword", credential.hash_password_);

    //And sending it formatted in JSON language
    boost::asio::write(sock_, boost::asio::buffer(auth_message.ToJSON()));
    // we sent the Auth message, we will shutdown in order to tell the server that we sent all
    sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

    //Now we use an extensible buffer for the unknown size response
    boost::asio::streambuf response_buf;

    //Now let's wait the response using theAcceptClient same technique in the server
    //(we will shut down the sending part on the server)
    //prompting an eof error
    boost::system::error_code ec;
    boost::asio::read(sock_, response_buf, ec);
    if (ec != boost::asio::error::eof) {
        //TODO controlla errori
    }
    //We handle the response parsing a string and then printing it.
    //Read the response_buf using an iterator and store it in a string
    //TODO might be an easier method to do this
    std::string response_json( (std::istreambuf_iterator<char>(&response_buf)), std::istreambuf_iterator<char>() );

    //Now we parsed the request and we use the string in order to create the corresponding ControlMessage
    ControlMessage response_message{response_json};
    if(response_message.type_ == 51){// it means we are actually dealing with a auth response (what we were expecting)
        //TODO return true false accordingly to the body of the control message received for now we just check it is an auth response
        // later on we will check the result
        // auth = true /false
        std::string auth_response = response_message.GetElement("auth");

        if(auth_response=="true") {

            if(DEBUG) std::cout << "\n\nUser " << Config::get_Instance()->ReadProperty("username") << " successfully authenticated."
                      << std::endl;
            return true;
        }

    }

    //If we don't go to return true, there is a problem so return false
    return false;
}

