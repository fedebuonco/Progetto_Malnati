#include <async_accept_client.h>
#include <filesystem>
#include <utility>

AsyncAcceptClient::AsyncAcceptClient(boost::asio::io_service& ios, unsigned short port_num, std::filesystem::path server_path) :
server_path_(std::move(server_path)),
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

    m_acceptor.async_accept(*sock,
                            [this, sock](
                                    const boost::system::error_code& ec)
                            {
                               onAccept(ec, sock);
                            });
}

void AsyncAcceptClient::onAccept(const boost::system::error_code& ec, const std::shared_ptr<boost::asio::ip::tcp::socket>& sock)
{

    (new AsyncService(sock, server_path_))->StartHandling(ec);

    // Init next async accept operation if acceptor has not been stopped yet.
    if (!m_isStopped.load()) {
        InitAccept();
    }
    else {
        // Stop accepting incoming connections and free allocated resources.
        m_acceptor.close();
    }
}