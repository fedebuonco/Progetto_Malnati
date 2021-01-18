//
// Created by fede on 12/11/20.
//

#ifndef SERVER_ASYNC_ACCEPT_CLIENT_H
#define SERVER_ASYNC_ACCEPT_CLIENT_H
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include "async_service.h"
class AsyncAcceptClient {

public:
    AsyncAcceptClient(boost::asio::io_service& ios, unsigned short port_num, std::filesystem::path server_path);
    void Start();
    void Stop();

private:
    void InitAccept();
    void onAccept(const boost::system::error_code& ec,
                  std::shared_ptr<boost::asio::ip::tcp::socket> sock);

private:
    std::filesystem::path server_path_;
    boost::asio::io_service& m_ios;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::atomic<bool> m_isStopped;

};


#endif //SERVER_ASYNC_ACCEPT_CLIENT_H
