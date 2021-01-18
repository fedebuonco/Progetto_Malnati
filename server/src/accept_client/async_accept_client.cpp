//
// Created by fede on 12/12/20.
//

#include <async_accept_client.h>
#include <filesystem>

AsyncAcceptClient::AsyncAcceptClient(boost::asio::io_service& ios, unsigned short port_num, std::filesystem::path server_path) :
server_path_(server_path),
m_ios(ios),
m_acceptor(m_ios,
           boost::asio::ip::tcp::endpoint(
                   boost::asio::ip::address_v4::any(),
                   port_num)),
m_isStopped(false)
{}

void AsyncAcceptClient::Start() {
    m_acceptor.listen();
    InitAccept();
}

// Stop accepting incoming connection requests.
void AsyncAcceptClient::Stop() {
    m_isStopped.store(true);
}

void AsyncAcceptClient::InitAccept() {
    std::shared_ptr<boost::asio::ip::tcp::socket>
            sock(new boost::asio::ip::tcp::socket(m_ios));

    //TODO How are thread assigned to each async_accept
    m_acceptor.async_accept(*sock.get(),
                            [this, sock](
                                    const boost::system::error_code& error)
                            {
                                std::cout << "Async_accepted callback called" <<    std::endl;
                                onAccept(error, sock);
                            });
}

void AsyncAcceptClient::onAccept(const boost::system::error_code& ec,
              std::shared_ptr<boost::asio::ip::tcp::socket> sock)
{

    (new AsyncService(sock, server_path_))->StartHandling();

    // Init next async accept operation if
    // acceptor has not been stopped yet.
    if (!m_isStopped.load()) {
        InitAccept();
    }
    else {
        // Stop accepting incoming connections
        // and free allocated resources.
        m_acceptor.close();
    }
}