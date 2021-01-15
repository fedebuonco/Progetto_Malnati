#include <async_service.h>
#include <filesystem>
#include <server.h>
#include "../../includes/database/database.h"



AsyncService::AsyncService(std::shared_ptr<boost::asio::ip::tcp::socket> sock, std::filesystem::path server_path) :
server_path_(server_path),
m_sock(sock)
        {
            first_sip_ = true;
        }

void AsyncService::StartHandling() {

    std::cout << "StartHandling chiamata " << std::endl;

    m_sock.get()->async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
                                  [this](boost::system::error_code ec, size_t bytes)
                                  {
                                      // Here we check if we have more to read.
                                      // if we don't we will get an ec signaling EOF
                                      // we must also look out for ec.value coming from other possible errors.
                                      // if ec.value == 2 is EOF we can call the onFinish
                                      if (!ec.value()) {
                                          onRequestReceived(ec, bytes);
                                      }
                                      else if (ec.value() == 2 ) { // EOF
                                          std::cout << ec.value() << ec.message() << std::endl;
                                          onFinish();
                                      }
                                  });

}

void AsyncService::onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    // Process the request.

    //We check if this is the first sip, if it is it contains metadata, and we must parse it.
    if(first_sip_){

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
        // We check if the folder does exist by querying the db.
        Database db;
        std::string user_path_str = db.getUserPath(user, server_path_);
        std::filesystem::path user_path = std::filesystem::path(user_path_str);
        std::filesystem::path filepath = server_path_ / "backupFiles" / "backupROOT" / user_path / file_string;
        std::filesystem::path filepath2 = server_path_ / "backupFiles" / "backupROOT" / user_path / file_string;
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
    //We close the file and compute its hash in order to comapre it with the provided one.
    m_outputFile.close();
    //TODO compute hash and compare it to hash_
    //TODO if ok insert it in the db with the filename, hash_ and lmt_
    //TODO notify client that we have the correct file and the filesipper can modify the client db in the client.

    delete this;
}


