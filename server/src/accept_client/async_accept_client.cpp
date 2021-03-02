#include <async_accept_client.h>
#include <filesystem>
#include <utility>

/**
 * Async accept client constructor
 * @param ios : Used for setting up the asynchronous callbacks
 * @param port_num : Port number
 * @param server_path : Path of the user's server exe
 */
AsyncAcceptClient::AsyncAcceptClient(boost::asio::io_service& ios, unsigned short port_num, std::filesystem::path server_path) :
server_path_(std::move(server_path)),
m_ios(ios),
m_acceptor(m_ios,
           boost::asio::ip::tcp::endpoint(
                   boost::asio::ip::address_v4::any(),
                   port_num)),
m_isStopped(false)
{}

/**
 * Starts the asynchronous server
 */
void AsyncAcceptClient::Start() {
    m_acceptor.listen();
    InitAccept();
}

/**
 * Stop accepting incoming connection requests.
 */
void AsyncAcceptClient::Stop() {
    m_isStopped.store(true);
}

/**
 * Setup for incoming request using the callback when the request arrived
 */
void AsyncAcceptClient::InitAccept() {
    std::shared_ptr<boost::asio::ip::tcp::socket> sock = std::make_shared<boost::asio::ip::tcp::socket>(m_ios);

    m_acceptor.async_accept(*sock,
                            [this, sock](
                                    const boost::system::error_code& ec)
                            {
                               onAccept(ec, sock);
                            });
}

/**
 * Takes the request and spawn an AsyncService that will start handling it
 * @param ec : error code provided by InitAccept
 * @param sock : active socket for handling client messages
 */
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