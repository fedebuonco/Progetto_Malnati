//
// Created by fede on 12/11/20.
//

#include <filesystem>
#include "async_server.h"

AsyncServer::AsyncServer(std::filesystem::path server_path) : server_path_(server_path){};

void AsyncServer::Start(unsigned short port_num, unsigned int thread_pool_size) {

    // Create and start Acceptor.
    acc.reset(new AsyncAcceptClient(m_ios, port_num, server_path_));
    acc->Start();

    // Create specified number of threads and
    // add them to the pool.
    for (unsigned int i = 0; i < thread_pool_size; i++) {
        std::unique_ptr<std::thread> th(
                new std::thread([this]()
                                {
                                    m_ios.run();
                                }));

        m_thread_pool.push_back(std::move(th));
    }
}

void AsyncServer::Stop() {
    acc->Stop();
    m_ios.stop();
    std::cout<<"Starting to shutdown Asynchronous Server..."<<std::endl;
    for (auto& th : m_thread_pool) {
        th->join();
    }
    std::cout<<"Successfully to shutdown Asynchronous Server..."<<std::endl;
}