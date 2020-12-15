//
// Created by fede on 12/10/20.
//

#include <file_sipper.h>
#include <string>
#include <sync_tcp_socket.h>
#include <iostream>

FileSipper::FileSipper(RawEndpoint re, const std::string &path) :
    sock_(ios_) ,
    ep_(boost::asio::ip::address::from_string(re.raw_ip_address), re.port_num),
    path_(path)
    {
        sip_counter = 0;
        std::cout << "Creating FileSipper for file " <<  path << std::endl;
        sock_.open(ep_.protocol());
        Connect();
        ios_.run();
    }

//TODO iterator endpoint not single...
/// We async connect to the specified server.
void FileSipper::Connect() {
    std::cout << "Connecting to "<<  ep_.address() <<" for file " <<  path_ << std::endl;
    sock_.async_connect(ep_, [this](boost::system::error_code ec)
    {    std::cout << "Currently in the async_connect callback " << std::endl;
        //TODO check if we still want to send the file
        OpenFile();
        Sip(ec_);
    });


}

// Opens a file
void FileSipper::OpenFile() {
    std::cout << "Opening File" << std::endl;
    files_stream_.open(path_.c_str(), std::ios_base::binary);
    if (files_stream_.fail())
        throw std::fstream::failure("Failed while opening file " + path_);

    //retrieve the byte size
    files_stream_.seekg(0, files_stream_.end);
    auto fileSize = files_stream_.tellg();
    this->file_size_ = fileSize;
    std::cout << "File Opened, Size : " << file_size_ << std::endl;
    //reset the stream;
    files_stream_.seekg(0, files_stream_.beg);
}

// Takes a sip of a file and sends it.
void FileSipper::Sip(const boost::system::error_code& t_ec){
    if (!t_ec) {
        // These checks for failbit or badbit(hardware & software fail)
        if (files_stream_) {
            // We try to read the stream into the buf_array
            files_stream_.read(buf_array_.data(), buf_array_.size());
            // Here we check if the read was successful
            // If we read but the file was EOF then the failbit will be triggered
            // Also the oef will be triggered, So here we check for another error
            // That could have triggered the failbit apart from the eof.
            if (files_stream_.fail() && !files_stream_.eof()) {
                auto msg = "Failed while reading file";
                std::cerr << msg << std::endl;
                throw std::fstream::failure(msg);
            }
            // Here we have the sip and can write it.
            std::cout << "WriteBuffer called for sip N :" << sip_counter << std::endl;
            sip_counter++;
            auto buf = boost::asio::buffer(buf_array_.data(), static_cast<size_t>(files_stream_.gcount()));
            writeBuffer(buf);
        }
        else{
            // Here we are if failbit or badbit are set to 1.
            // The failbit could have been set by the eof, so we check it
            if (files_stream_.eof()){
                std::cout << "EOF Reached!" << std::endl;
            } else { // Here if we had a different problem that made us fail! we trhrow exception
                auto msg = "Failed while reading file";
                std::cerr << msg << std::endl;
                throw std::fstream::failure(msg);
            }
            ios_.stop();
            return;
        }
    } else {
        std::cerr <<"Error "<< t_ec.value() << " -- "<< t_ec.message() <<std::endl;
    }

}
