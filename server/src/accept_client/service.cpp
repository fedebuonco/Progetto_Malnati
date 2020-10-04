//
// Created by fede on 10/4/20.
//

#include "service.h"

/// Starts handling the particular client that requested a service. Spawns a thread that actually handle the request and detach it
/// \param sock TCP socket conneted to the client
void Service::ReadRequest(std::shared_ptr<asio::ip::tcp::socket> sock) {
    std::thread th(([this, sock] () {HandleClient(sock);}));
    th.detach();
}

void Service::HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock) {
    //TODO handle the request based on the type if auth do something if tree do another thing.
    // for now assumes that always a auth request

    // Auth request read

    boost::asio::streambuf request_buf;
    boost::system::error_code ec;

    boost::asio::read(*sock, request_buf, ec);
    // This checks if the client has finished writing
    if (ec != boost::asio::error::eof){
        //qua se non ho ricevuto la chiusura del client
        std::cout<<"DEBUG: NON ho ricevuto il segnale di chiusura del client";
        throw boost::system::system_error(ec);
    }

    std::string message;
    std::istream input_stream(&request_buf);
    std::getline(input_stream, message);

    std::cout << "Ho letto  " << message << std::endl;

    std::string uguale="0";

    if(message=="MARCO MARCO"){
        uguale="1";
    }

    boost::asio::write(*sock, boost::asio::buffer(uguale));
    // Send the eof error shutting down the server.
    sock->shutdown(boost::asio::socket_base::shutdown_send);
    //TODO fare ciclo qui si chiude il server
}
