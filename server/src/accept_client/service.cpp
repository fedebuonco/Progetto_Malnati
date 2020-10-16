//
// Created by fede on 10/4/20.
//

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <control_message.h>
#include "service.h"

/// Starts handling the particular client that requested a service. Spawns a thread that actually handle the request and detach it
/// \param sock TCP socket conneted to the client
void Service::ReadRequest(std::shared_ptr<asio::ip::tcp::socket> sock) {
    std::thread th(([this, sock] () {HandleClient(sock);}));
    th.detach();
}

/// Real handling starts here. Should distinguish auth requests and tree requests
void Service::HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock) {

    //Here we read the request -> parse it in a ptree -> use the ptree to build the ControlMessage and then we
    //Switch case in order to correctly handle it.

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
    //TODO might be an easier method to do this
    std::string request_json( (std::istreambuf_iterator<char>(&request_buf)), std::istreambuf_iterator<char>() );
    //DEBUG
    std::cout << "Ho letto  " << request_json << std::endl;

    //Now we parsed the request and we use the ptree object in  order to create the corresponding ControlMessage
    ControlMessage request_message{request_json};

    // Here based on the type of the message we switch accordingly.
    switch (request_message.type_) {
        case 1:{             //AUTH REQUEST
            //TODO real checkIdentity and control message
            // TODO change control message constructor for now in the client we will only check that is 51 not if auth = true/false
            ControlMessage check_result{51};
            boost::asio::write(*sock, boost::asio::buffer(check_result.ToJSON()));
            // Send the eof error shutting down the server.
            //TODO qua magicamente va ignorato l'errore GRAVISSIMO
            sock->shutdown(boost::asio::socket_base::shutdown_both, ec);
            break;
        }
        case 2:{            //TREE REQUEST
            //TODO starts computing tree and sends it back
            std::cout<< " Here we call treeCompute()"<< std::endl;
            break;
        }

    }

    // Now the service class was instantiated in the heap &
    // As the service class has finished it's work we are gonna delete it here
    // "delete this" is known to be bad code, but if we follow some security
    // advices we can use it.
    delete this;
}
