#include <async_service.h>
#include <filesystem>
#include <server.h>
#include <sha.h>
#include <hex.h>
#include <files.h>
#include "../../includes/database/database.h" //TODO FIX this

// _WIN32 = we're in windows
#ifdef _WIN32
// Windows
#define MAX_LENGTH 260
#else
// Not windows
#define MAX_LENGTH 4096
#endif


AsyncService::AsyncService(std::shared_ptr<boost::asio::ip::tcp::socket> sock, std::filesystem::path server_path) :
server_path_(server_path),
m_sock(sock)
        {
            first_sip_ = true;
        }

void AsyncService::StartHandling(const boost::system::error_code& ec) {

    if (ec){
        //If we have an error, we dont waste time handling the service and just return.
        return;
    }

    m_sock->async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
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
                                          // File arrived
                                          onFinish();

                                      } else{ //Here if the error
                                          std::cerr << "Error on async_read" << std::endl;
                                          delete this;
                                      }

                                  });

}

void AsyncService::onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred) {
    // Process the request.

    //We check if this is the first sip, if it is it contains metadata, and we must parse it.
    if(continue_writing) {

        if (first_sip_) {
            ParseMetadata(m_buf.data());
            // we now check that we have a valid user.
            Database authentication;

            if (!authentication.auth(received_user_, received_hpass_, server_path_)) {
                //The sip is not authenticated, we discard the file
                std::cerr << "Error on user name " << received_user_ << std::endl;
                return;
            }

            first_sip_ = false;
            // We check if the folder does exist by querying the db.
            Database db;
            std::string user_path_str = db.getUserPath(received_user_, server_path_);
            std::filesystem::path user_path = std::filesystem::path(user_path_str);
            created_file_path_ = server_path_ / "backupFiles" / "backupROOT" / user_path / received_file_string_;

            std::filesystem::path filepath2 =
                    server_path_ / "backupFiles" / "backupROOT" / user_path / received_file_string_;

            std::error_code ec2;
            bool created = std::filesystem::create_directories(filepath2.remove_filename(), ec2);
            if (ec2) {
                //If the parent folder of the files over 260 character
                //In a real server the 260 character limit will be lifted
                std::cerr << "Error path reached over 260 character" << ec2 << " " << created << std::endl;
                continue_writing=false;
                filename_too_long = true;
                return;
            }

            m_outputFile.open(created_file_path_, std::ios_base::binary);
            if (m_outputFile.fail()) {
                //Reach the limit of character for Linux or Windows
                if (created_file_path_.string().size() >= MAX_LENGTH) {
                    std::cerr << "File reached max length for the OS" << std::endl;
                    continue_writing=false;
                    filename_too_long = true;
                }

                //Generic error during open, try again

                //return;
            }


            //File has first_sip valid, we start recieving it.
            std::cout << "\n[INSERT STARTED]["<< received_user_ <<"] File " << this->received_file_string_ << " uploading" << std::endl;

        } else {

            m_outputFile.write(m_buf.data(), bytes_transferred);
            if (m_outputFile.fail()) { // We couldn't write the file
                std::cerr << "While writing, the stream failed" << std::endl;
                continue_writing=false;

                //return;
            }
            //we clear the array for the next call
            m_buf.fill('\000');
        }
    }
    // We go on to the next read.
    StartHandling(ec);
}

void AsyncService::onFinish() {
    //We close the file and compute its hash in order to compare it with the provided one.
    m_outputFile.close();
    //Compute hash and compare it to hash_ to see if the file is corrupted
    CryptoPP::SHA256 hash;
    std::string digest;

    if(!continue_writing && filename_too_long){
        onAbort();
        return;
    }

    try {
        CryptoPP::FileSource f(
                created_file_path_.c_str(),
                true,
                new CryptoPP::HashFilter(hash,
                                         new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));

    } catch(std::exception& e){
        // The digest could not be computed we therefore put a wrong digest on purpose.
        digest = "COULDNOTBECOMPUTED";
    }

    bool isOkay= false;

    if(received_hash_== digest) {
        Database db;
        //If in the meantime we receive a delete request we check again that the files still exists
        if(std::filesystem::exists(created_file_path_)){
            db.insertFile(received_user_, received_file_string_, received_lmt_, server_path_);
            isOkay=true;
        }
    }
    else {
        //File was corrupted so we delete the file
        std::error_code ec;
        auto result = std::filesystem::remove( created_file_path_, ec );
        if (result) {
            std::cout << "\n[REMOVE OF CORRUPT FILE]["<< received_user_ <<"] File " << this->received_file_string_ << std::endl;
        }

    }

    //We send the OK, but we flush the buffer before.
    m_buf.fill('\000');

    if(isOkay) m_buf[0] = '1';
    else m_buf[0] = '0';

    m_sock->async_write_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
                                  [this, isOkay](boost::system::error_code ec, size_t bytes){
                                      if (!ec.value()) {
                                          if(isOkay) std::cout << "\n[INSERT COMPLETED]["<< received_user_ <<"] File " << this->received_file_string_ << " successfully received and saved" << std::endl;
                                          else       std::cout << "\n[INSERT NOT COMPLETED]["<< received_user_ <<"] File " << this->received_file_string_ << " was not saved and the client was notified" << std::endl;
                                          delete this;
                                      }
                                  });

}

void AsyncService::onAbort() {

    m_buf.fill('\000');
    m_buf[0] = '2';

    // We send to the client the request to change the status to NOTSENT, as the file can't be sent using our app,
    // as the filename is too long.

    m_sock->async_write_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
                             [this](boost::system::error_code ec, size_t bytes){
                                 if (!ec.value()) {
                                    delete this;
                                 }
                             });



}

void AsyncService::ParseMetadata(const std::string& metadata){
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

    if (received_user_.length() < 2)
        std::cout << "RROR" << std::endl;

    current = metadata.find(received_hpass_,previous);
    received_file_string_ = metadata.substr(previous, current - previous);
    previous = current + 64;

    if (received_user_.length() < 2)
        std::cout << "RROR" << std::endl;
}

