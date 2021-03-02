#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <async_service.h>

/**
 * Class that handle request that arrive in the asynchronous client
 */
class AsyncAcceptClient {

    void InitAccept();
    void onAccept(const boost::system::error_code& ec, const std::shared_ptr<boost::asio::ip::tcp::socket>& sock);

    ///Path of the user's server exe
    std::filesystem::path server_path_;

    ///Used for setting up the asynchronous callbacks
    boost::asio::io_service& m_ios;

    ///Wait the request from client and will provide to the server an active socket for the communication
    boost::asio::ip::tcp::acceptor m_acceptor;

    ///Atomic variable used to stop the async server
    std::atomic<bool> m_isStopped;

public:
    AsyncAcceptClient(boost::asio::io_service& ios, unsigned short port_num, std::filesystem::path server_path);
    void Start();
    void Stop();

};

