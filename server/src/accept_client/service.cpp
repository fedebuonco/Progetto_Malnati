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
    //TODO handle the request
}
