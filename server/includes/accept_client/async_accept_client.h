#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <async_service.h>

class AsyncAcceptClient {

    void InitAccept();
    void onAccept(const boost::system::error_code& ec, const std::shared_ptr<boost::asio::ip::tcp::socket>& sock);

    std::filesystem::path server_path_;
    boost::asio::io_service& m_ios;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::atomic<bool> m_isStopped;

public:
    AsyncAcceptClient(boost::asio::io_service& ios, unsigned short port_num, std::filesystem::path server_path);
    void Start();
    void Stop();

};

