#include <async_service.h>
#include <filesystem>

// TODO make sure that an asyncService manage a single file.
// TODO

AsyncService::AsyncService(std::shared_ptr<boost::asio::ip::tcp::socket> sock) :
m_sock(sock)
        {
            first_sip_ = true;
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

    //We check if this is the first sip, if it is it contains metadata, and we must parse it.
    if(first_sip_){
        //TODO open the file folder and
        //TODO filename is actually a path
        file_name_ = m_buf.data();
        first_sip_ = false;
        // We check if the folder does exist
        std::filesystem::path filepath = std::filesystem::path(file_name_);
        std::filesystem::create_directories(filepath.remove_filename());
        m_outputFile.open(file_name_, std::ios_base::binary);
        StartHandling();
        return;
    }

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
    //TODO FAI ClOSE DEL FILE
    // TODO SHould check file integrity and send to the client the response if it is ok or not
    //
    delete this;
}


