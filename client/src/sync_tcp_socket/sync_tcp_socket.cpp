#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <config.h>
#include <sync_tcp_socket.h>

/// Initialize socket, endpoint and parse the raw ip. After that opens the socket using the specified protocol, TCP.
/// \param raw_ip_add Parsed raw ip
/// \param port_n port connecting to
SyncTCPSocket::SyncTCPSocket(const std::string& raw_ip_add, unsigned short port_n) :
                                           sock_(ios_) ,
                                           ep_(boost::asio::ip::address::from_string(raw_ip_add), port_n) {

    boost::system::error_code errorCode;
    sock_.open(ep_.protocol(),errorCode);
    if (errorCode)
    {
        // Error - Open Socket
        std::cerr << "Error opening the socket" << errorCode.message();
        std::exit( EXIT_FAILURE );
    }
}

/// Shutdown both part(sending & receiving) and closes the socket giving back the resource to the system.
SyncTCPSocket::~SyncTCPSocket() {

    boost::system::error_code ec;
    sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    sock_.close(ec);
    if(ec){
        std::cerr<< "Error during shutdown " <<  ec.message() <<std::endl;
        std::exit( EXIT_FAILURE );
    }

}

/// Connect the socket to the server. Tries the connection for a specified number of times
/// \param n_tries Number of re tries done.
void SyncTCPSocket::ConnectServer(int n_tries) {
    while (n_tries) {
        try {

            if (DEBUG)  std::cout << "\nEstablishing connection to server " << ep_.address() <<":"<<ep_.port() <<std::endl ;

            sock_.connect(ep_);
            break; //We exit from while loop because the connection was successful
        }
        catch (boost::system::system_error &e) {
            n_tries--;

            //We wait before try again
            std::this_thread::sleep_for (std::chrono::seconds (1));

            if(n_tries == 0){
                std::cerr << "RawEndpoint Problem - Server not responding or wrong IP address/Port (" << e.code().value() << ")" <<std::endl ;
                std::exit(EXIT_FAILURE);
            }
        }
        catch (std::exception& e) {
            std::cerr << "Generic Error during server connection" << std::endl ;
            std::exit(EXIT_FAILURE);
        }
    }
}



