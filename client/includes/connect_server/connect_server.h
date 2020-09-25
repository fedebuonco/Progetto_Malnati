//
// Created by fede on 9/21/20.
//

#ifndef CLIENT_CONNECT_SERVER_H
#define CLIENT_CONNECT_SERVER_H
#include <boost/asio.hpp>

/// ConnectServer - establish a connection and auth
class ConnectServer {
private:
    // ordine di queste 3 istruzioni deve essere cosi.
    //TODO scoprire come mai?
    boost::asio::io_service ios_;
    boost::asio::ip::tcp::endpoint ep_;
    boost::asio::ip::tcp::socket sock_;

public:
    ConnectServer(const std::string& raw_ip_add,unsigned short port);

    bool Authenticate();

   ~ConnectServer();
};





#endif //CLIENT_CONNECT_SERVER_H
