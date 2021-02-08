#pragma once

#include <boost/asio.hpp>

/// Wrapper of a synchronous tcp socket.
/// Will be used for MessageControls and Auth
class SyncTCPSocket {
private:

    //Do not invert the order of this two instruction
    boost::asio::io_service ios_;
    boost::asio::ip::tcp::endpoint ep_;
public:
    SyncTCPSocket(const std::string& raw_ip_add, unsigned short port);
    ~SyncTCPSocket();
    void ConnectServer(int n_tries);
    //TODO: Implement the three rules   @marco

    /// Boost socket
    boost::asio::ip::tcp::socket sock_;
};

