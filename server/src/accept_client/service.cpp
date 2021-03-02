#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <control_message.h>
#include <filesystem>
#include <tree_t.h>
#include <server.h>
#include <../../includes/database/database.h>
#include <service.h>


/// Starts handling the particular client that requested a service. Spawns a thread that actually handle the request and detach it
/// \param sock TCP socket connected to the client
void Service::ReadRequest(const std::shared_ptr<asio::ip::tcp::socket>& sock, const std::filesystem::path& serverP) {
    this->serverPath=serverP;

    //Spawns a thread that handle the request
    std::thread th(([this, sock] () {
             HandleClient(sock);
    }));

    //Detach this thread
    th.detach();
}

/// Real handling starts here. Should distinguish auth requests and tree requests

/***
 * This function handle the client request. First check if the message is authenticated and then
 * handle the request according to the message type
 * @param sock: active socket created by SpawnSession method of AcceptClient class.
 */
void Service::HandleClient(const std::shared_ptr<asio::ip::tcp::socket>& sock) {
    //Now we parsed the request and we use the ptree object in order to create the corresponding ControlMessage

    try{
        //We parse the incoming request into a request message
        ControlMessage request_message = SyncReadCM(sock);

        //A message with type 5 means that we don't want to handle the message (like we are shutting down the server)
        if(request_message.type_==5){
            //The detach thread will be kill when read this message
            return;
        }

        //We check that the message is authenticated
        if (!CheckAuthenticity(request_message)){

            //The request is not accepted because the message is not authenticated
            //Return to the client a control message with 'type:51' and 'auth:false'
            ControlMessage check_result{51};
            check_result.AddElement("auth", "false");

            SyncWriteCM(sock, check_result);
        }

        //Here the message is authenticated. We handle the request according to the message type.
        switch (request_message.type_) {
            case 1:{
                //Type:1    --> client requests to authenticate the user

                //We return to the client the information that the user is authenticated because we check it earlier.
                ControlMessage check_result{51};
                check_result.AddElement("auth", "true");

                SyncWriteCM(sock, check_result);
                break;
            }    
        
            case 2: {
                //Type:2    --> client requests the TREE & TIME of the server

                //Let's start building the Response Control Message
                ControlMessage treet_result{52};

                //Take the username of the person who made the request from the request message.
                std::string username = request_message.GetElement("Username");

                Database db;
                //Find inside the DB the name of the server's folder in which we have stored the users file.
                std::string user_folder_name = db.getUserPath(username, this->serverPath);


                //First we check if the program has the usersTREE folder.
                //In this folder we create a DB for each user containing the user file's information.
                std::filesystem::directory_entry users_tree{this->serverPath / "backupFiles" / "usersTREE"};
                //If the folder doesn't exist, we create the directory (i.e. only when we start the server for the first time).
                if (!users_tree.exists()) std::filesystem::create_directories(this->serverPath / "backupFiles" / "usersTREE");

               //Now we check if we have the user folder inside backupROOT.
               //In this folder we will store all the user's file

               std::filesystem::directory_entry user_directory_path{this->serverPath / "backupFiles" / "backupROOT" / user_folder_name};

                if (!user_directory_path.exists()) {
                    std::filesystem::create_directories(user_directory_path);
                    std::cout << "User doesn't have the folder. I create a new folder with name: " << user_folder_name << std::endl;

                    //Create DB file for the user TREE inside usersTREE folder.
                    db.createTable(user_folder_name, this->serverPath );
                }

                this->associated_user_path_ = user_directory_path;

                //We create the TREE & TIME and put this information inside the response message
                TreeT server_treet(user_directory_path, this->serverPath);
                treet_result.AddElement("Tree", server_treet.genTree());
                treet_result.AddElement("Time", server_treet.genTimes());

                SyncWriteCM(sock, treet_result);
                break;
            }
            case 3: {
                //Type:3    --> client requests to delete a file

                //Let's start building the Response Control Message
                ControlMessage treet_result{53};

                //Take the username of the person who made the request from the request message.
                std::string username = request_message.GetElement("Username");

                Database db;
                //Find inside the DB the name of the server's folder in which we have stored the users file.
                std::string user_folder_name = db.getUserPath(username, this->serverPath);

                //Take the list of files to be deleted
                std::string list_to_be_deleted = request_message.GetElement("To_be_deleted");

                //For each file we delete the file itself and the row inside user tree DB
                std::string file;
                std::istringstream stream_tbd{list_to_be_deleted};
                while(std::getline(stream_tbd, file)){
                    //Delete the file
                    std::filesystem::path user_folder_path = this->serverPath / "backupFiles" / "backupROOT" / user_folder_name;
                    std::filesystem::path file_path = this->serverPath / "backupFiles" / "backupROOT" / user_folder_name / file ;
                    std::error_code ec;

                    // First we remove the file
                    auto results = std::filesystem::remove(file_path, ec);

                    if(results) std::cout << "\n[DELETE]["<< username <<"] "<<  file << " successfully deleted" << std::endl;


                    // Then we check if it is the last file in the folder, if it is we delete it
                    // we perform this operation recursively in order to delete all empty folders.
                    std::filesystem::path path_iterator = file_path.parent_path();
                    while (std::filesystem::exists(path_iterator) && std::filesystem::is_empty(path_iterator)){

                        if (path_iterator == user_folder_path){
                            break;
                        }
                        // remove this folder and go to parent folder
                        std::filesystem::remove(path_iterator, ec);

                        path_iterator = path_iterator.parent_path();
                    }
                    //We don't care if the file exists or not; we try anyway to delete the row inside DB
                    db.deleteFile(user_folder_name, file, this->serverPath);
                }

                break;
            }
            default: {
                if(DEBUG) std::cout << "Service.cpp SWITCH; Default case" << std::endl;
                break;
            }

        }

    }
    catch (boost::system::system_error& e){
        std::cerr<<"Boost system error" << e.what()  << std::endl;
        delete this;
        return;
        //Now this thread will die because is detached
    }
    catch(std::exception& e){
        std::cerr<<"Error handling request message" << e.what()  << std::endl;
        delete this;
        return;
        //Now this thread will die because is detached
    }

    // Now the service class was instantiated in the heap &
    // As the service class has finished it's work, we are gonna delete it here
    // "delete this" is known to be bad code, but if we follow some security
    // advices we can use it.
    delete this;
}

/// We check that the message is authenticated
/// \param cm: Control Message read
/// \return true if authenticated, false otherwise
bool Service::CheckAuthenticity(const ControlMessage& cm){
    Database authentication;
    return authentication.auth(cm.username_, cm.hashkey_, this->serverPath);
}

/// Used to write the Control Message in a syncronous way.
/// \param sock The socket I'm writing the Control Message to.
/// \param cm The control message to be written.
void Service::SyncWriteCM(const std::shared_ptr<asio::ip::tcp::socket>& sock, ControlMessage& cm){

    boost::system::error_code ec;

    //We write and close the send part of the SyncTCPSocket, in order to tell the server that we have finished writing
    boost::asio::write(*sock, boost::asio::buffer(cm.ToJSON()), ec);

    if (ec){
        if(DEBUG) std::cerr<<"Error writing Control Message; message: " << ec.message() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    sock->shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);

    if (ec){
       if(DEBUG) std::cerr<<"Error writing Control Message; message: " << ec.message() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/// Used for reading until the EOF on a given socket, then we return a ControlMessage using the buffer we read
/// \param sock The socket we are reading on.
/// \return The Control Message that we read.
ControlMessage Service::SyncReadCM(const std::shared_ptr<asio::ip::tcp::socket>& sock){
    // We read until the eof, then we return a ControlMessage using the buffer we read.
    boost::asio::streambuf request_buf;

    boost::system::error_code ec;
    boost::asio::read(*sock, request_buf, ec);

    // This checks if the client has finished writing
    if (ec != boost::asio::error::eof){
       if(DEBUG) std::cerr<<"Unable to read the control message arrived from client." << ec.message() << std::endl;
       //5 means that the server doesn't handle the message
       ControlMessage cm{5};
       return cm;
    }
    //Read the response_buf using an iterator and store it in a string In order to store it in a ControlMessage
    std::string response_json((std::istreambuf_iterator<char>(&request_buf)), std::istreambuf_iterator<char>() );

    //Create a control message with the response
    ControlMessage cm{response_json};
    return cm;
}