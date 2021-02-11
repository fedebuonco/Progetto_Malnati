#include <file_sipper.h>
#include <string>
#include <iostream>
#include <utility>
#include <database.h>

FileSipper::FileSipper(const RawEndpoint& re, std::filesystem::path folder_watched, std::filesystem::path db_path,
                       std::string username, const std::string& hashed_pass,   std::filesystem::path file_path,
                       std::string file_string, std::string hash, std::string lmt)  :
        sock_(ios_) ,
        ep_(boost::asio::ip::address::from_string(re.raw_ip_address), re.port_num),
        folder_watched_(std::move(folder_watched)),
        db_path_(std::move(db_path)),
        path_(std::move(file_path)),
        hash_(std::move(hash)),
        lmt_(std::move(lmt)),
        file_string_(std::move(file_string)),
        username_(std::move(username)),
        sip_counter(0)
{
    //We build the metadata to put into the first sipper of the fileSipper (metadata are separated with the user hashed password)
    metadata_ = hash_ + hashed_pass + lmt_ + hashed_pass + username_ + hashed_pass + file_string_;
    //if(DEBUG) std::cout << "Creating FileSipper for file " <<  path_.string() << std::endl;
    //if(DEBUG) std::cout << "With metadata =  " << metadata_ << std::endl;
    sock_.open(ep_.protocol());
}


/// Methods that starts the sending of the File.
void FileSipper::Send(){
    //We change the fileSipper status to true, to indicate that we handle the fileSipper
    status.store(true);
    Connect();
    ios_.run();
}

//TODO iterator endpoint not single...
/// We async connect to the specified server.
void FileSipper::Connect() {
    //if(DEBUG) std::cout << "Connecting to "<<  ep_.address() <<" for file " <<  path_.string() << std::endl;

    sock_.async_connect(ep_, [this](boost::system::error_code ec) {
        //if(DEBUG) std::cout << "Currently in the async_connect callback " << std::endl;
        //TODO check if we still want to send the file. In teoria non serve perchè fatto in patch
        OpenFile();
        FirstSip(ec_);

    });
}

/// Opens a file and prepare things for the FirstSip
void FileSipper::OpenFile() {

    //We open the file
    files_stream_.open(path_.c_str(), std::ios_base::binary);
    if (files_stream_.fail())
        throw std::fstream::failure("Failed while opening file " + path_.string());

    //Retrieve the byte size
    files_stream_.seekg(0, files_stream_.end);
    auto fileSize = files_stream_.tellg();
    this->file_size_ = fileSize;
    //std::cout << "File Opened, Size : " << file_size_ << std::endl;

    //Reset the stream;
    files_stream_.seekg(0, files_stream_.beg);
}

/// Takes the file and send its metadata to the server.
/// \param t_ec
void FileSipper::FirstSip(const boost::system::error_code& t_ec){

    if (files_stream_) {
        // We create the first sip by sending file metadata
        int i =0;
        // I paste the metadata in the buffer in this format
        buf_metadata.fill('\000');
        for( auto letter : metadata_){
            buf_metadata[i] = letter;
            i++;
        }
        //then we delimit the end using the terminator char
        buf_metadata[i] = '\000';
        // And send it
        auto buf = boost::asio::buffer(buf_metadata.data(),1024);
        writeBuffer(buf);
    }
    else{
        // Here we are if fail bit or bad bit are set to 1.
        // The fail bit could have been set by the eof, so we check it
        if (files_stream_.eof()){
            //std::cout << "EOF Reached!" << std::endl;

        } else { // Here if we had a different problem that made us fail! we throw exception
            auto msg = "Failed while reading file";
            std::cerr << msg << std::endl;
            throw std::fstream::failure(msg);
        }
        ios_.stop();
        return;
    }
}

/// Takes a sip of a file and sends it.
void FileSipper::Sip(const boost::system::error_code& t_ec){
    if (!t_ec) {
        // These checks for fail bit or bad bit (hardware & software fail)
        if (files_stream_) {
            // We try to read the stream into the buf_array
            files_stream_.read(buf_array_.data(), buf_array_.size());
            // Here we check if the read was successful
            // If we read but the file was EOF then the fail bit will be triggered
            // Also the oef will be triggered, So here we check for another error
            // That could have triggered the fail bit apart from the eof.
            if (files_stream_.fail() && !files_stream_.eof()) {
                auto msg = "Failed while reading file";
                std::cerr << msg << std::endl;
                throw std::fstream::failure(msg);
            }
            // Here we have the sip and can write it.
            //std::cout << "WriteBuffer called for sip N :" << sip_counter << std::endl;
            sip_counter++;
            auto buf = boost::asio::buffer(buf_array_.data(), static_cast<size_t>(files_stream_.gcount()));
            writeBuffer(buf);
        }
        else{
            // Here we are if fail bit or bad bit are set to 1.
            // The fail bit could have been set by the eof, so we check it
            if (files_stream_.eof()){
                //std::cout << "EOF Reached!" << std::endl;
                // We can call WaitOk where we wait for the ok from the server;
                files_stream_.close();
                sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
                //std::cout << "Send  :" << this->file_string_ << std::endl;
                WaitOk();
            } else { // Here if we had a different problem that made us fail, we throw exception
                auto msg = "Failed while reading file";
                std::cerr << msg << std::endl;
                throw std::fstream::failure(msg);
            }
            //ios_.stop();

            return;
        }
    } else {
        std::cerr <<"Error "<< t_ec.value() << " -- "<< t_ec.message() <<std::endl;
        //TODO Here error in the sending WriteBuffer   FEDE HELP US VA BENE USCIRE ?
        std::exit(EXIT_FAILURE);

    }
}

void FileSipper::WaitOk() {
    buf_metadata.fill('\000');
    //sock_.read_some(boost::asio::buffer(buf_metadata.data(), buf_metadata.size()));

    async_read(sock_, boost::asio::buffer(buf_metadata.data(), buf_metadata.size()),
               [this](boost::system::error_code ec, size_t bytes) {
                   // Here based on the checksum result we modify the database.
                   int checksum_ok = buf_metadata[0] - '0';
                   UpdateFileStatus(db_path_, folder_watched_, file_string_, checksum_ok);
                   ios_.stop();
               });

};

void FileSipper::UpdateFileStatus(std::filesystem::path db_path ,std::filesystem::path folder_watched,std::string file_string, int check) {
    DatabaseConnection db(db_path, folder_watched);
    if (!check){ // we need to retry sending the file has the server did not receive the file in a correct way
        try {
            db.ChangeStatusToNew(file_string);
            status.store(false);
        } catch (...) {
            //std::cout<<" WaitOk "<<std::endl;
        }
        //TODO se vogliamo farlo rifare a lui devo fare puntatore a socket per poi ditruggerlo e ricrearlo.
        //this->Send();
    } else { // here if the server has received the correct file.
        try {
            db.ChangeStatusToSent(file_string);

            status.store(false);
        }catch(...){ //TODO: Check this thing
            //std::cout<<" WaitOk "<<std::endl;
        }
    }


}


