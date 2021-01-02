//
// Created by fede on 12/11/20.
//

#ifndef SERVER_ASYNC_SERVER_H
#define SERVER_ASYNC_SERVER_H

#include <memory>
#include <boost/asio/io_service.hpp>
#include <thread>
#include "async_accept_client.h"

class AsyncServer{
public:

    // Start the server.
    void Start(unsigned short port_num, unsigned int thread_pool_size);
    // Stop the server.
    void Stop();

private:
    boost::asio::io_service m_ios;
    std::unique_ptr<AsyncAcceptClient> acc;
    std::vector<std::unique_ptr<std::thread>> m_thread_pool;


};
#endif //SERVER_ASYNC_SERVER_H
