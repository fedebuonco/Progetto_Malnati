#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <service.h>
#include <accept_client.h>
#include <filesystem>

/// Represents an high level acceptor. Accepts and instantiate a Service.
/// When constructed it instantiates an acceptor socket and starts listening on it.
class AcceptClient{
private:
    boost::asio::io_service& ios_;
    boost::asio::ip::tcp::acceptor acceptor_;
    /// Path of the server.exe
    std::filesystem::path serverPath;
public:
    AcceptClient(boost::asio::io_service& ios, unsigned short port_num, std::filesystem::path serverPath);
    void SpawnSession(const std::atomic<bool>& stop);
};