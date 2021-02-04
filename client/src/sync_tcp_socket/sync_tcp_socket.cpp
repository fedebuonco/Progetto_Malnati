#include <iostream>
#include <string>
//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
#include <boost/asio.hpp>
#include <config.h>
#include <sync_tcp_socket.h>

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

            //if (DEBUG)  std::cout << "\nEstablishing connection to server " << ep_.address() <<":"<<ep_.port() <<std::endl ;

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



