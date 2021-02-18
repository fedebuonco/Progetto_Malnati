#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <boost/asio/streambuf.hpp>
#include <fstream>
#include <filesystem>

class AsyncService {

    std::filesystem::path server_path_;
    std::shared_ptr<boost::asio::ip::tcp::socket> m_sock;
    std::string m_response;
    boost::asio::streambuf m_request;

    enum { MaxLength = 1024 };
    std::array<char, MaxLength> m_buf;
    boost::asio::streambuf m_requestBuf_;
    std::ofstream m_outputFile;
    size_t m_fileSize;
    int read_counter;
    bool first_sip_;

    std::string metadata_;
    std::string received_hash_;
    std::string received_user_;
    std::string received_hpass_;
    std::string received_lmt_;
    std::string received_file_string_;

    std::filesystem::path created_file_path_;

    bool continue_writing = true;

    void onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void onFinish();
    void createFile();
    void ParseMetadata(const std ::string& metadata);
    void onAbort();
    void ConsumeSipper();

public:
    AsyncService(std::shared_ptr<boost::asio::ip::tcp::socket> sock, std::filesystem::path server_path);
    void StartHandling(const boost::system::error_code& ec);


};


