#include <file_sipper.h>
#include <string>
#include <iostream>
#include <utility>
#include <database.h>
#include <client.h>

/// Used for "sipping" the file in order to send it to the server. It splits the file in 1024 byte and asynchronously writes it in the stream.
/// \param re Raw Endpoint containing the server address
/// \param folder_watched Folder watched by the monitor.
/// \param db_path Path of the database file
/// \param username Username, used for metadata retrieval.
/// \param hashed_pass Hashed SHA-256 Password.
/// \param file_path Full path of the file.
/// \param file_string String of the path relative to the folder being watched.
/// \param hash Digest of the file (SHA-256).
/// \param lmt Last modified time of the file.
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




/// Methods that starts the async_send of the file. When finished the callback passed will be called.
/// \param rem_call The callback that will be executed when the async_send will complete.
void FileSipper::Send(std::function<void()> rem_call){
    // We change the fileSipper status to true, to indicate that we handle the fileSipper
    status.store(true);
    remove_callback_ = std::move(rem_call);
    Connect();
    ios_.run();
}

/// We async connect to the specified server.
void FileSipper::Connect() {
    //if(DEBUG) std::cout << "Connecting to "<<  ep_.address() <<" for file " <<  path_.string() << std::endl;
    sock_.async_connect(ep_, [this](boost::system::error_code ec) {
        //Here if we can't connect we simply exit, but we make sure to change the status of the file back to new in the DB
        if (ec) { // we simulate a bad sent file, (checksum result == 0 )
            UpdateFileStatus(db_path_, folder_watched_, file_string_, 0);
            throw std::runtime_error("Could Not Connect to File Server");
        }
        // Here if the connection was successful
        OpenFile();
        FirstSip();

    });
}

/// Opens a file and prepare things for the FirstSip
void FileSipper::OpenFile() {

    //We open the file
    files_stream_.open(path_.c_str(), std::ios_base::binary);
    if (files_stream_.fail()) { // We couldn't open the file, we check its existence.
        if(std::filesystem::exists(path_)){ //we had some problem opening the file, but it still exists.
                                            // We exit but change the status back to NEW
            UpdateFileStatus(db_path_, folder_watched_, file_string_, 0);
            throw std::runtime_error("File couldn't be opened. It will set back to NEW");

        }
        else{ //File doesn't even exists anymore, we can exit safely;
            throw std::runtime_error("File couldn't be opened. It does not exist anymore.");
        }
    }

}

void FileSipper::WriteSip(boost::asio::mutable_buffers_1 buffer_sip) {
    boost::asio::async_write(sock_,
                             buffer_sip,
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

/// Takes the file and send its metadata to the server.
/// \param t_ec
void FileSipper::FirstSip(){
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

        WriteSip(boost::asio::buffer(buf_metadata.data(),1024));

        //writeBuffer(buf);
    }
    else{
        if(std::filesystem::exists(path_)){ //we had some problem opening the file, but it still exists.
            // We exit but change the status back to NEW
            UpdateFileStatus(db_path_, folder_watched_, file_string_, 0);
            throw std::runtime_error("File couldn't be opened. It will set back to NEW");

        }else{ //File doesn't even exists anymore, we can exit safely;
            throw std::runtime_error("File couldn't be opened. It does not exist anymore.");
        }
    }
}

/// Takes a sip of a file and ask to write it invoking the WriteBuffer.
/// \param t_ec Error code passed from the previous WriteBuffer invocation.
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
                files_stream_.close();
                sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
                UpdateFileStatus(db_path_, folder_watched_, file_string_, 0);
                throw std::runtime_error("Sip read failed. The file is set back to NEW.");
            }
            // Here we have the sip and can write it, we also increment the sip_counter for debug purposes.
            sip_counter++;
            // We write the buffer using what we read. The amount of data we read is retrieved using the gcount.

            WriteSip(boost::asio::buffer(buf_array_.data(), static_cast<size_t>(files_stream_.gcount())));

        }
        else{
            // Here we are if fail bit or bad bit are set to 1.
            // The fail bit could have been set by the eof, so we check it
            if (files_stream_.eof()){

                // We can call WaitOk where we wait for the ok from the server
                files_stream_.close();
                sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
                WaitOk();

            } else { // Here if we had a different problem that made us fail, we throw exception
                files_stream_.close();
                sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
                UpdateFileStatus(db_path_, folder_watched_, file_string_, 0);
                throw std::runtime_error("Sip read failed. The file is set back to NEW.");
            }

            return;
        }
    }
    else { // If we have any error on writing the sip we just exit, setting the status to NEW again
        files_stream_.close();
        sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
        UpdateFileStatus(db_path_, folder_watched_, file_string_, 0);
        throw std::runtime_error("Sip interrupted. The file is set back to NEW.");
    }
}

/// Waits for the checksum response of the server, that after having computed the hash of the file, sends to the client the result of the comparison.
/// Using that we update the status of the file on the db.
void FileSipper::WaitOk() {
    buf_metadata.fill('\000');

    async_read(sock_, boost::asio::buffer(buf_metadata.data(), buf_metadata.size()),
               [this](boost::system::error_code ec, size_t bytes) {

                   if (ec){ // We must simulate worst scenario, so we act like the checksum was not ok

                       UpdateFileStatus(db_path_, folder_watched_, file_string_, 0);

                   } else { //Here if we got no error while reading

                       int checksum_ok = buf_metadata[0] - '0';
                       UpdateFileStatus(db_path_, folder_watched_, file_string_, checksum_ok);

                   }

                   ios_.stop();  // THIS STOPS further async calls for this socket.

                   remove_callback_();

               });
}

/// Updates the status of the filesipper on the db. Depending on the integer passed we decide the status.
/// \param db_path Path of the db file.
/// \param folder_watched Folder monitored by the watcher.
/// \param file_string File path in string format relative to the folder watched.
/// \param check The integer we pass in order to decide the status.
void FileSipper::UpdateFileStatus(const std::filesystem::path& db_path , const std::filesystem::path& folder_watched, const std::string& file_string, int check) {
    DatabaseConnection db(db_path, folder_watched);
    if (check==1)//1
        db.ChangeStatusToSent(file_string);
    else if(check==0) //0
        db.ChangeStatusToNew(file_string);
    else {  //2
        std::cerr << "Unable to save the \"" << file_string << "\" in the cloud" << std::endl;
        db.ChangeStatusToNotSent(file_string);
    }

    auto all_sent = db.AllSent();

    if(all_sent) {

        std::filesystem::path folder_path = folder_watched / "XAW";
        std::filesystem::create_directory(folder_path);
        std::filesystem::remove(folder_path);

    }

}
