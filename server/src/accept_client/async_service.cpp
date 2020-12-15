#include <async_service.h>

AsyncService::AsyncService(std::shared_ptr<boost::asio::ip::tcp::socket> sock) :
m_sock(sock)
        {
            m_outputFile.open("Progetto-Malnati.zip", std::ios_base::binary);
        }

void AsyncService::StartHandling() {

    std::cout << "StartHandling chiamata " << std::endl;

    m_sock.get()->async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
                                  [this](boost::system::error_code ec, size_t bytes)
                                  {
                                      if (!ec.value()) {
                                          onRequestReceived(ec, bytes);
                                      }
                                      else {
                                          std::cout << "Errore" << std::endl;
                                          onFinish();
                                      }
                                  });

}

void AsyncService::onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    // Process the request.

    m_outputFile.write(m_buf.data(), bytes_transferred);

    //we clear the array for the next call

    m_buf.fill('\0');

    // We check if this is the last sip, if it is
    // we can call the onFinish()
    // else we call the StartHandling again,
    // as one service will read an entire file before dying.

    StartHandling();

    return;

}

void AsyncService::onFinish() {
    delete this;
}


