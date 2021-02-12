#pragma once

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

    std::filesystem::path serverPath;

    std::filesystem::path associated_user_path_;

    void HandleClient(const std::shared_ptr<asio::ip::tcp::socket>& sock);
    ControlMessage SyncReadCM(const std::shared_ptr<asio::ip::tcp::socket>& sock);
    void SyncWriteCM(const std::shared_ptr<asio::ip::tcp::socket>& sock, ControlMessage& cm);
    bool CheckAuthenticity(const ControlMessage& cm);

public:

    void ReadRequest(const std::shared_ptr<asio::ip::tcp::socket>& sock,const std::filesystem::path& serverP);
};


