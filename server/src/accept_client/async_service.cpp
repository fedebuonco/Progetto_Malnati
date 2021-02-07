#include <async_service.h>
#include <filesystem>
#include <server.h>

#include <sha.h>
#include <hex.h>
#include <files.h>

#include "../../includes/database/database.h"

void AsyncService::ParseMetadata(std ::string metadata){

    // Now on metadata_  i will parse everything
    // metadata_ = hash_ + hashed_pass + lmt + hashed_pass + username_ + hashed_pass + file_string_;

    //first we get the hashed_pass knowing that hash_ has a fixed size.

    received_hpass_ = metadata.substr(64,64);


    std::size_t current, previous = 0;

    current = metadata.find(received_hpass_,previous);
    received_hash_ = metadata.substr(previous, current - previous);
    previous = current + 64;

    current = metadata.find(received_hpass_,previous);
    received_lmt_ = metadata.substr(previous, current - previous);
    previous = current + 64;

    current = metadata.find(received_hpass_,previous);
    received_user_ = metadata.substr(previous, current - previous);
    previous = current + 64;

    current = metadata.find(received_hpass_,previous);
    received_file_string_ = metadata.substr(previous, current - previous);
    previous = current + 64;


}

AsyncService::AsyncService(std::shared_ptr<boost::asio::ip::tcp::socket> sock, std::filesystem::path server_path) :
server_path_(server_path),
m_sock(sock)
        {
            first_sip_ = true;
        }

void AsyncService::StartHandling() {

    m_sock.get()->async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
                                  [this](boost::system::error_code ec, size_t bytes)
                                  {
                                      // Here we check if we have more to read.
                                      // if we don't we will get an ec signaling EOF
                                      // we must also look out for ec.value coming from other possible errors.
                                      // if ec.value == 2 is EOF we can call the onFinish
                                      //std::cout << ec.value() << ec.message() << std::endl;
                                      if (!ec.value()) {
                                          onRequestReceived(ec, bytes);
                                      }
                                      else if (ec.value() == 2 ) { // EOF
                                          std::cout << ec.value() << ec.message() << std::endl;
                                          onFinish();
                                      }

                                      //todo delete se ho altri tipi di problemi strani.

                                  });

}

void AsyncService::onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    // Process the request.

    //We check if this is the first sip, if it is it contains metadata, and we must parse it.
    if(first_sip_){

        ParseMetadata(m_buf.data());
        // we now check that we have a valid user.
        Database authentication;

        if (!authentication.auth(received_user_, received_hpass_, server_path_)){
            // Here if we dont find a user
            //TODO: Decidere cosa fare se user non è presente (praticamente mai si verificherà);
        }

        first_sip_ = false;
        // We check if the folder does exist by querying the db.
        Database db;
        std::string user_path_str = db.getUserPath(received_user_, server_path_);
        std::filesystem::path user_path = std::filesystem::path(user_path_str);
        created_file_path_ = server_path_ / "backupFiles" / "backupROOT" / user_path / received_file_string_;

        std::filesystem::path filepath2 = server_path_ / "backupFiles" / "backupROOT" / user_path / received_file_string_;
        std::filesystem::create_directories(filepath2.remove_filename());

        m_outputFile.open(created_file_path_, std::ios_base::binary);
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


    //Compute hash and compare it to hash_ to see if the file is corrupted
    CryptoPP::SHA256 hash;
    std::string digest;

    try {
        CryptoPP::FileSource f(
                created_file_path_.c_str(),
                true,
                new CryptoPP::HashFilter(hash,
                                         new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));

        // We print the digest
        if(DEBUG) std::cout << "Digest is = " << digest << std::endl;


    } catch(std::exception& e){
        std::cerr <<"ERROR " <<
                  e.what() << std::endl;
        //TODO Gestire impossibiltà di hashing.     Proviamo un'altra volta a farlo?

        //TODO gestire errori db

        ///*****************************************************************************
    }

    bool isOkay=false;
    if(received_hash_==digest) {
        //TODO if ok insert it in the db with the received_file_string_ , received_hash_ and received_lmt_
        Database db;
        db.insertFile(received_user_, received_file_string_, received_hash_, received_lmt_, server_path_);
        isOkay=true;
    }
    else {
        //File was corrupted so we delete the file
        std::error_code ec;
        std::filesystem::remove( created_file_path_, ec );
        if(ec) {
            std::cerr << "Error deleting file: ec " << ec << std::endl;
            //TODO: Controllare
            //TODO: RETURN?
        }
    }


    //We send the OK
    m_buf.fill('\000');
    if(isOkay) m_buf[0] = '1';
    else m_buf[0] = '0';
    m_sock.get()->async_write_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
                                  [this](boost::system::error_code ec, size_t bytes){
                                      if (!ec.value()) {
                                          std::cout << "Sent OK!" << std::endl;
                                      }
                                  });

    delete this;
}


