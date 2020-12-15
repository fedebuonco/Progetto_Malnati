//
// Created by fede on 12/10/20.
//

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

///Encapsulates a file+hash that need to be sent to a server.
/// The file is sent in small "sips" and after each sending we call a callback.
/// Offers a callback that, after performing some checks, calls for another
/// sip, thus creating a chain of sent sips.
class FileSipper {
    std::string path_;
    std::string hash_;

    boost::asio::io_service ios_;
    boost::asio::ip::tcp::endpoint ep_;
    boost::asio::ip::tcp::socket sock_;
    boost::system::error_code ec_;
    std::ifstream files_stream_;
    enum { MessageSize = 1024 };
    std::array<char, MessageSize> buf_array_;
    int file_size_;
    int sip_counter;

public:
    FileSipper(RawEndpoint re, std::string const& path);

private:
    void OpenFile();
    void Connect();
    void Sip(const boost::system::error_code& t_ec);

    template<class Buffer>
    void writeBuffer(Buffer& t_buffer)
    {
        boost::asio::async_write(sock_,
                                 t_buffer,
                                 [this](boost::system::error_code ec, std::size_t)
                                 {
                                     Sip(ec);
                                 });

    }



};


#endif //CLIENT_FILE_SIPPER_H
