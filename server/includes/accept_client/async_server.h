#pragma once

#include <memory>
#include <boost/asio/io_service.hpp>
#include <thread>
#include <async_accept_client.h>

class AsyncServer{

    std::filesystem::path server_path_;
    boost::asio::io_service m_ios;
    std::unique_ptr<AsyncAcceptClient> acc;
    std::vector<std::unique_ptr<std::thread>> m_thread_pool;

public:
    explicit AsyncServer(std::filesystem::path server_path);

    // Start the server.
    void Start(unsigned short port_num, unsigned int thread_pool_size);

    // Stop the server.
    void Stop();

};

