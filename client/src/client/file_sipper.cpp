#include <file_sipper.h>
#include <string>
#include <sync_tcp_socket.h>
#include <iostream>

FileSipper::FileSipper(RawEndpoint re, const std::string &path) :
    sock_(ios_) ,
    ep_(boost::asio::ip::address::from_string(re.raw_ip_address), re.port_num),
    path_(path),
    sip_counter(0)
    {
        if(DEBUG) std::cout << "Creating FileSipper for file " <<  path << std::endl;
        sock_.open(ep_.protocol());
        Connect();
        ios_.run();
    }

//TODO iterator endpoint not single...
/// We async connect to the specified server.
void FileSipper::Connect() {
    if(DEBUG) std::cout << "Connecting to "<<  ep_.address() <<" for file " <<  path_ << std::endl;

    sock_.async_connect(ep_, [this](boost::system::error_code ec) {
        if(DEBUG) std::cout << "Currently in the async_connect callback " << std::endl;
        //TODO check if we still want to send the file. In teoria non serve perchè fatto in patch
        //TODO Handle exception
        OpenFile();
        FirstSip(ec_);
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


/// Takes the file and send its metadata to the server.
/// \param t_ec
void FileSipper::FirstSip(const boost::system::error_code& t_ec){

    // TODO We need to retrieve the entry from the the db (path, hash, lmt, STATUS)
    // Ne abbiamo bisogno perchè il server deve rifare hash e controllare
    // lmt ci serve perchè il server deve memorizzare quello più nuovo
    // and save the hash and lmt to a string.

    //TODO: Devo partire sempre?

    if (files_stream_) {
        // We create the first sip by sending file metadata
        int i =0;
        // I paste the metadata in the buffer in this format
        // FILENAME@HASH@LMT
        buf_metadata.fill('\000');

        // We start costrugting the first sip, name of the file, then hash then lmt.
        //name
        for( auto letter : path_){
            buf_metadata[i] = letter;
            i++;
        }
        //TODO hash
        //TODO lmt

        //then we delimit the end using the terminator char
        buf_metadata[i] = '\000';

        // And send it
        auto buf = boost::asio::buffer(buf_metadata.data(),1024);
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
        //TODO Here error in the sending WriteBuffer
    }
}

bool FileSipper::isReady() const {
    return ready;
}

void FileSipper::setReady(bool ready) {
    FileSipper::ready = ready;
}







