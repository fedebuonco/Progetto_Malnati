//
// Created by fede on 9/21/20.
//
#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "service.h"
#include "accept_client.h"

#define BACKLOG_SIZE 10

/// Represents an high level acceptor. Accepts and instantiate a Service.
/// When constructed it instantiates an acceptor socket and starts listening on it.
///
class AcceptClient{
private:
    boost::asio::io_service& ios_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::string serverPath;
public:
    AcceptClient(boost::asio::io_service& ios, unsigned short port_num, std::string s);
    void SpawnSession();


};