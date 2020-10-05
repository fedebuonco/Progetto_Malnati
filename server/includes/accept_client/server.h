//
// Created by fede on 10/4/20.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H


#include <thread>
#include <atomic>
#include <boost/asio.hpp>

/// Creates a server that listen on a specific port.
class Server {
private:
    std::unique_ptr<std::thread> thread_;
    boost::asio::io_service ios_;
    std::atomic<bool> stop_;
public:
    Server();
    void Start(unsigned short port_num);
    void Stop();
private:
    void Run(unsigned short port_num);
};


#endif //SERVER_SERVER_H
