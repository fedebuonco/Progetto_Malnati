//
// Created by fede on 10/4/20.
//

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "service.h"

/// Starts handling the particular client that requested a service. Spawns a thread that actually handle the request and detach it
/// \param sock TCP socket conneted to the client
void Service::ReadRequest(std::shared_ptr<asio::ip::tcp::socket> sock) {
    std::thread th(([this, sock] () {HandleClient(sock);}));
    th.detach();
}

/// Real handling starts here. Should distinguish auth requests and tree requests
void Service::HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock) {
    // TODO handle the request based on the type if auth do something if tree do another thing.

    boost::asio::streambuf request_buf;
    boost::system::error_code ec;

    boost::asio::read(*sock, request_buf, ec);
    // This checks if the client has finished writing
    if (ec != boost::asio::error::eof){
        //qua se non ho ricevuto la chiusura del client
        std::cout<<"DEBUG: NON ho ricevuto il segnale di chiusura del client";
        throw boost::system::system_error(ec);
    }

    //Read the request_buf using an iterator and store it in a string
    std::string request_json( (std::istreambuf_iterator<char>(&request_buf)), std::istreambuf_iterator<char>() );
    //DEBUG
    std::cout << "Ho letto  " << request_json << std::endl;

    // Now we have the request, let's parse it
    std::stringstream ss;
    ss << request_json;
    boost::property_tree::ptree request_ptree;
    boost::property_tree::read_json(ss, request_ptree);

    //Now we parsed the request
    int type;
    type = request_ptree.get<int>("Type");
    std::string success;
    if (type == 1){
        //AUTH
        success = "1";
        boost::asio::write(*sock, boost::asio::buffer(success));
        // Send the eof error shutting down the server.
        sock->shutdown(boost::asio::socket_base::shutdown_send);
    } else {
        // NON AUTH
        success = "0";
        boost::asio::write(*sock, boost::asio::buffer(success));
        // Send the eof error shutting down the server.
        sock->shutdown(boost::asio::socket_base::shutdown_send);
    }

    //Now the service class was instantiated in the heap so someone should deallocate it.
    //As the service class has finished it's work we are gonna do it here
    delete this;

    //TODO fare ciclo qui si chiude il server
    //TODO controllare come ridare al sistema il socket. Lo aveva istanziato accept_client ora che è stato utilizzato
    // lo potrebbe deallocare qua. Bisogna vedere se sock shutdown è un deallocatore
}
