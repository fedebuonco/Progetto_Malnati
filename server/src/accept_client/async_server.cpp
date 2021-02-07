#include <filesystem>
#include <memory>
#include <utility>
#include <async_server.h>

AsyncServer::AsyncServer(std::filesystem::path server_path) : server_path_(std::move(server_path)){};

void AsyncServer::Start(unsigned short port_num, unsigned int thread_pool_size) {

    // Create and start Acceptor.
    acc = std::make_unique<AsyncAcceptClient>(m_ios, port_num, server_path_);
    acc->Start();

    // Create specified number of threads and add them to the pool.
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

    //Wait all threads of the thread_pool before close the async server
    for (auto& th : m_thread_pool) {
        th->join();
    }

}