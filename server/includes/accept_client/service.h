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

class Service {
private:
    std::shared_ptr<asio::ip::tcp::socket> sock_;
    asio::streambuf request_buf_;


public:
    Service(std::shared_ptr<asio::ip::tcp::socket> sock) : sock_(sock){};
    void ReadRequest();

private:
    void OnRequestReceived(){
        //TODO error check

    }


};


#endif //SERVER_SERVICE_H
