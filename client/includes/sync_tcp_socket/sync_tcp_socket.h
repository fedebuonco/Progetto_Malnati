//
// Created by fede on 9/21/20.
//

#ifndef CLIENT_SYNC_TCP_SOCKET_H
#define CLIENT_SYNC_TCP_SOCKET_H
#include <boost/asio.hpp>

/// SyncTCPSocket - establish a connection and auth
class SyncTCPSocket {
private:
    // ordine di queste 3 istruzioni deve essere cosi.
    //TODO scoprire come mai?
    boost::asio::io_service ios_;
    boost::asio::ip::tcp::endpoint ep_;
    boost::asio::ip::tcp::socket sock_;

public:
    SyncTCPSocket(const std::string& raw_ip_add, unsigned short port);

    bool Authenticate();

   ~SyncTCPSocket();

    void ConnectServer();
};





#endif //CLIENT_SYNC_TCP_SOCKET_H