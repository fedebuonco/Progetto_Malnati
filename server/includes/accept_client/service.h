//
// Created by fede on 10/4/20.
//

#ifndef SERVER_SERVICE_H
#define SERVER_SERVICE_H
#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

/// Class responsible for handling a single client by reading it's request message, processing it and then answering
/// Represents thus, a single service provided by the server application
class Service {

private:
    std::shared_ptr<asio::ip::tcp::socket> sock_;
    asio::streambuf request_buf_;
    std::string serverPath;

public:
    Service(){};
    void ReadRequest(std::shared_ptr<asio::ip::tcp::socket> sock,std::string s);

private:
    // The Destructor is made private, as the only way to delete a service
    // is by suicide (delete this) after the client is handled
    ~Service(){};
    void HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock);
};


#endif //SERVER_SERVICE_H
