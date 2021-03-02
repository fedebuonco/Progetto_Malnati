#pragma once

#include <memory>
#include <boost/asio/io_service.hpp>
#include <thread>
#include <async_accept_client.h>

/**
 * Asynchronous server for handling file management
 */
class AsyncServer{

    ///Path of the user's server exe
    std::filesystem::path server_path_;

    ///Used for setting up the asynchronous callbacks
    boost::asio::io_service m_ios;

    ///Wait the request from client and will provide to the server an active socket for the communication
    std::unique_ptr<AsyncAcceptClient> acc;

    ///Thread pool for receiving and storing the files
    std::vector<std::unique_ptr<std::thread>> m_thread_pool;

public:
    explicit AsyncServer(std::filesystem::path server_path);

    // Start the server.
    void Start(unsigned short port_num, unsigned int thread_pool_size);

    // Stop the server.
    void Stop();

};

