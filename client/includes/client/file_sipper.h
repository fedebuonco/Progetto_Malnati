#ifndef CLIENT_FILE_SIPPER_H
#define CLIENT_FILE_SIPPER_H

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
/// Offers a callback that, after performing some checks, calls for another
/// sip, thus creating a chain of sent sips.
class FileSipper {
    //FileSipper is made of path_ and hash_
    std::filesystem::path path_;
    std::filesystem::path folder_watched_;
    std::filesystem::path db_path_;
    std::string hash_;
    std::string lmt_;
    std::string metadata_;
    std::string username_;

    boost::asio::io_service ios_;
    boost::asio::ip::tcp::endpoint ep_;
    boost::asio::ip::tcp::socket sock_;
    boost::system::error_code ec_;
    std::ifstream files_stream_;

    //The file will be sent in small portions of this size
    enum { MessageSize = 1024 };

    std::array<char, MessageSize> buf_array_;
    std::array<char, MessageSize> buf_metadata;

    //Dimension of the file to send
    int file_size_;
    //Number of sip (i.e. block) in which the file is split
    int sip_counter;

    bool ready;

public:
    FileSipper(RawEndpoint re, std::filesystem::path folder_watched, std::filesystem::path db_path, std::string username, std::filesystem::path file_path, std::string file_string, std::string hash, std::string lmt);
    void Send();

    std::string file_string_;

    std::atomic<bool> status = false;
private:
    void OpenFile();
    void Connect();
    void Sip(const boost::system::error_code& t_ec);
    void FirstSip(const boost::system::error_code& t_ec);
    void WaitOk();



    template<class Buffer>
    void writeBuffer(Buffer& t_buffer)
    {
        boost::asio::async_write(sock_,
                                 t_buffer,
                                 [this](boost::system::error_code ec, std::size_t size)
                                 {
                                     //Let's see the status of the sip, in order to see if this is the last one.
                                     //std::cout << "STATUS async write: " << ec.value() << " written " << size << std::endl;
                                     Sip(ec);

                                 });

    }



};


#endif //CLIENT_FILE_SIPPER_H
