#pragma once


#include <boost/asio/io_service.hpp>
#include <string>
#include <boost/asio/ip/tcp.hpp>
#include <config.h>
#include <fstream>
#include <boost/asio/streambuf.hpp>
#include <sync_tcp_socket.h>
#include <iostream>

/// Encapsulates a file+hash that need to be sent to a server.
/// The file is sent in small "sips" and after each sending we call a callback.
/// Offers a callback that, after performing some checks, calls for another sip, thus creating a chain of sent sips.
class FileSipper {
    //FileSipper is made of path_ and hash_

    /// Path of the file that will be sent.
    std::filesystem::path path_;
    /// Path of the folder monitored.
    std::filesystem::path folder_watched_;
    /// Path of the DB file.
    std::filesystem::path db_path_;
    /// hash Digest of the file (SHA-256).
    std::string hash_;
    /// Last modified time of the file.
    std::string lmt_;
    /// String containg the first sip.
    std::string metadata_;
    /// Username of the user requesting the backup of the file.
    std::string username_;

    boost::asio::io_service ios_;
    boost::asio::ip::tcp::endpoint ep_;
    boost::asio::ip::tcp::socket sock_;
    /// Stream used for opening the file, we will take sips from it and async send them to the server.
    std::ifstream files_stream_;

    //The file will be sent in small portions of this size
    enum { MessageSize = 1024 };

    /// Buffer used for the incoming sips (contains data)
    std::array<char, MessageSize> buf_array_{};
    /// Buffer used for the first sip and the result of the hashing
    std::array<char, MessageSize> buf_metadata{};
    /// Number of sip (i.e. block) in which the file is split
    int sip_counter;

public:
    FileSipper(const RawEndpoint& re, std::filesystem::path folder_watched, std::filesystem::path db_path, std::string username, const std::string& hashed_pass,  std::filesystem::path file_path, std::string file_string, std::string hash, std::string lmt);
    void Send(std::function<void()> rem_call);

    virtual ~FileSipper();


    /// Name of the file
    std::string file_string_;
    /// Callback that will be called once the fileSipper receive the OK from the server.
    std::function<void()> remove_callback_;
    /// Flag used for knowing if the FileSipper has been activated.
    std::atomic<bool> status = false;
private:
    void OpenFile();
    void Connect();
    void Sip(const boost::system::error_code& t_ec);
    void FirstSip();
    void WaitOk();


    //TODO rendere non generica.
    template<class Buffer>
    void writeBuffer(Buffer& t_buffer)
    {
        boost::asio::async_write(sock_,
                                 t_buffer,
                                 [this](boost::system::error_code ec, std::size_t size)
                                 {
                                     if (ec) { // If we have any error on writing the sip we just exit, setting the status to NEW again
                                         files_stream_.close();
                                         sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
                                         UpdateFileStatus(db_path_, folder_watched_, file_string_, 0);
                                         throw std::runtime_error("Sip interrupted. The file is set back to NEW.");
                                     }
                                     // We go to the next sip.
                                     Sip(ec);
                                 });

    }


    void UpdateFileStatus(const std::filesystem::path& db_path, const std::filesystem::path& folder_watched, const std::string& file_string,
                          int check);


};
