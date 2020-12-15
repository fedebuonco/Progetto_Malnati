//
// Created by fede on 12/11/20.
//

#ifndef SERVER_ASYNC_SERVICE_H
#define SERVER_ASYNC_SERVICE_H

#include <boost/asio.hpp>
#include <iostream>
#include <boost/asio/streambuf.hpp>
#include <fstream>

class AsyncService {
public:
    AsyncService(std::shared_ptr<boost::asio::ip::tcp::socket> sock);
    void StartHandling();

private:
    void onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void onFinish();
    void createFile();


private:
    std::shared_ptr<boost::asio::ip::tcp::socket> m_sock;
    std::string m_response;
    boost::asio::streambuf m_request;
    enum { MaxLength = 1024 };
    std::array<char, MaxLength> m_buf;
    boost::asio::streambuf m_requestBuf_;
    std::ofstream m_outputFile;
    size_t m_fileSize;
    int read_counter;
    std::string m_fileName;
};

#endif //SERVER_ASYNC_SERVICE_H
