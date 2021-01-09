#ifndef SERVER_SERVICE_H
#define SERVER_SERVICE_H
#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>
#include <filesystem>
#include <control_message.h>


using namespace boost;

/// Class responsible for handling a single client by reading it's request message, processing it and then answering
/// Represents thus, a single service provided by the server application
class Service {

private:
    std::shared_ptr<asio::ip::tcp::socket> sock_;
    asio::streambuf request_buf_;
    std::filesystem::path serverPath;

public:
    //TODO: Secondo me non è necessario
    Service(){};
    void ReadRequest(std::shared_ptr<asio::ip::tcp::socket> sock,std::filesystem::path serverP);

private:
    // The Destructor is made private, as the only way to delete a service
    // is by suicide (delete this) after the client is handled
    ~Service(){};
    void HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock);
    ControlMessage SyncReadCM(std::shared_ptr<asio::ip::tcp::socket> sock);
    bool SyncWriteCM(std::shared_ptr<asio::ip::tcp::socket> sock, ControlMessage& cm);
    bool CheckAuthenticity(const ControlMessage& cm);
};


#endif //SERVER_SERVICE_H
