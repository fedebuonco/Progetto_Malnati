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
        //TODO Hash, time last modified

        metadata_ = m_buf.data();

        //Now on metadata_ i have the USER@HASH@LMT@FILENAME, i will parse everything
        std::size_t current, previous = 0;

        current = metadata_.find("@",previous);
        std::string user = metadata_.substr(previous, current - previous);
        previous = current + 1;

        current = metadata_.find("@",previous);
        std::string hash = metadata_.substr(previous, current - previous);
        previous = current + 1;

        current = metadata_.find("@",previous);
        std::string lmt = metadata_.substr(previous, current - previous);
        previous = current + 1;

        current = metadata_.find("@",previous);
        std::string file_string = metadata_.substr(previous, current - previous);
        previous = current + 1;





        first_sip_ = false;
        // We check if the folder does exist
        //TODO actually use user folder.
        std::filesystem::path user_folder = std::filesystem::path ("./Prova");
        std::filesystem::path filepath = user_folder / file_string;
        std::filesystem::path filepath2 = user_folder / file_string;
        std::filesystem::create_directories(filepath2.remove_filename());
        m_outputFile.open(filepath, std::ios_base::binary);
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
    //TODO SALVA FILE DENTRO LA CARTELLA marco_01
    //TODO: CHECK HASH CHE ARRIVA DAL CLIENT CON HASH CALCOLATO CON IL FILE SALVATO E VEDI SE SONO UGUALI
    //TODO: SE SONO UGUALI ****INSERT NEL DB**** DELL UTENTE
    //TODO SHould check file integrity and send to the client the response if it is ok or not

    delete this;
}


