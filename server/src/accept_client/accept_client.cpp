#include <iostream>
#include <boost/asio.hpp>
#include <service.h>
#include <accept_client.h>
#include <filesystem>
#include <utility>

volatile sig_atomic_t flag=0;

AcceptClient::AcceptClient(asio::io_service& ios, unsigned short port_num,std::filesystem::path serverPath) :
        ios_(ios),
        acceptor_(ios_,boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(),port_num)),
        serverPath(std::move(serverPath))
{
    acceptor_.listen();
}

/// Creates an active socket and accepts an incoming connection by the client thus linking them and creating a Service class
/// that will take care of the handling. This service class will obtain the pointer to the socket here created.
/// The accept is blocking and has a own list of incoming connections.
void AcceptClient::SpawnSession(const std::atomic<bool>& stop) {

    std::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(ios_));
    acceptor_.accept(*sock);

    //Until stop is false we handle client requests
    if(!stop.load())
        (new Service)->ReadRequest(sock, this->serverPath);
}


