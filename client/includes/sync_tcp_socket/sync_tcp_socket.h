//
// Created by fede on 9/21/20.
//

#ifndef CLIENT_SYNC_TCP_SOCKET_H
#define CLIENT_SYNC_TCP_SOCKET_H
#include <boost/asio.hpp>

/// Wrapper of a syncronous tcp socket.
/// Will be used for MessageControls and Auth
class SyncTCPSocket {
private:
    // ordine di queste 3 istruzioni deve essere cosi.
    //TODO scoprire come mai?
    boost::asio::io_service ios_;
    boost::asio::ip::tcp::endpoint ep_;
public:
    SyncTCPSocket(const std::string& raw_ip_add, unsigned short port);
    ~SyncTCPSocket();
    bool Authenticate();
    void ConnectServer(int n_tries);

    /// Boost socket
    boost::asio::ip::tcp::socket sock_;
};





#endif //CLIENT_SYNC_TCP_SOCKET_H
