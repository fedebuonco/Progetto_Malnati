#pragma once

#include <thread>
#include <atomic>
#include <boost/asio.hpp>
#include <filesystem>

extern bool DEBUG;

/// Creates a server that listen on a specific port.
class Server {

    std::unique_ptr<std::thread> thread_;
    boost::asio::io_service ios_;

    std::atomic<bool> stop_;
    std::filesystem::path serverPath;

    void Run(unsigned short port_num);

public:

    explicit Server(std::filesystem::path serverPath);
    void Start(unsigned short port_num);
    void Stop();

};

