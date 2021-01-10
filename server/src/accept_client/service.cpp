#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <control_message.h>
#include <filesystem>
#include <tree_t.h>
#include <server.h>
#include "../../includes/database/database.h"
#include "service.h"

/// Starts handling the particular client that requested a service. Spawns a thread that actually handle the request and detach it
/// \param sock TCP socket connected to the client
void Service::ReadRequest(std::shared_ptr<asio::ip::tcp::socket> sock,std::filesystem::path serverP) {
    this->serverPath=serverP;
    std::thread th(([this, sock] () {HandleClient(sock);}));
    th.detach();
}

/// Real handling starts here. Should distinguish auth requests and tree requests
void Service::HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock) {

    // Here we read the request
    // parse it in a ptree,
    // use the ptree to build the ControlMessage
    // we then check that the message is authenticated
    // and then we switch case in order to correctly handle it.

    //Now we parsed the request and we use the ptree object in order to create the corresponding ControlMessage

    try{
        //We parse the incoming request into a request message
        //TODO: Eccezione
        ControlMessage request_message = SyncReadCM(sock);


        if(request_message.type_==5){
            std::cout << "ENTRATI" << std::endl;
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

                //TODO Se si verifica un eccezione tra la creazione dello userfolder e il createTable (db utente)
                // vediamo la directory ma non vediamo il DB. Azione da fare è RIPROVA. Questo potrebbe creare problemi, consiglio di fare nella catch una politica
                // che cancella sia la cartella principale (tanto in questo punto è vuota) ed il db. Ricordo comunque che nel caso in cui
                // il db esiste quando entriamo sulla createTable droppo comunque la tabella e la ricreo.

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
                //std::string list_to_be_deleted = request_message.GetElement("Deleted");

                //For each file we delete the file itself and the row inside user tree DB
                //for(auto file : list_to_be_deleted ) {
                    //Delete the file
                    //std::filesystem::path file_path = this->serverPath / "backupFiles" / "backupROOT" / user_folder_name / file ;

                    //std::error_code ec;
                    //bool result = std::filesystem::remove(file_path, ec);

                        //TODO: This file doesn't exist or we have error
                        //if(!result)

                    //We doesn't care if the file exists or not; we try anyway to delete the row inside DB
                    //db.deleteFile(user_folder_name, list_to_be_deleted, this->serverPath);
                //}

                break;
            }
            default: {
                if(DEBUG) std::cout << "Service.cpp SWITCH; Default case" << std::endl;
                break;
            }

        }

    }
    catch (std::invalid_argument& e){
        std::cerr<<"CIAO !" << e.what()  << std::endl;
        //TODO: Qui andiamo quando terminiamo con ctrl e anche quando c'è un eccezione
        delete this;
        return;
    }
    catch(std::exception& e){
        std::cerr<<"Error handling request message" << e.what()  << std::endl;
        //TODO: Il thread ha una detach, con return dovrebbe terminate il thread perchè non c'è niente sopra. è corretto?
        delete this;
        return;
        //Now this thread will die because is detached
    }


    // Now the service class was instantiated in the heap &
    // As the service class has finished it's work we are gonna delete it here
    // "delete this" is known to be bad code, but if we follow some security
    // advices we can use it.
    delete this;
}

bool Service::CheckAuthenticity(const ControlMessage& cm){
    Database authentication;
    return authentication.auth(cm.username_, cm.hashkey_, this->serverPath);
}

bool Service::SyncWriteCM(std::shared_ptr<asio::ip::tcp::socket> sock, ControlMessage& cm){
    //We write and close the send part of the SyncTCPSocket, in order to tell the server that we have finished writing
    boost::asio::write(*sock, boost::asio::buffer(cm.ToJSON()));
    sock->shutdown(boost::asio::ip::tcp::socket::shutdown_send);
    return true;
}

ControlMessage Service::SyncReadCM(std::shared_ptr<asio::ip::tcp::socket> sock){
    // We read until the eof, then we return a ControlMessage using the buffer we read.
    boost::asio::streambuf request_buf;
    boost::system::error_code ec;
    boost::asio::read(*sock, request_buf, ec);

    // This checks if the client has finished writing
    if (ec != boost::asio::error::eof){
        std::cerr<<"DEBUG: NON SONO" << ec << std::endl;
        throw boost::system::system_error(ec);
    }
    //Read the response_buf using an iterator and store it in a string In order to store it in a ControlMessage
    // TODO might be an easier method to do this
    std::string response_json((std::istreambuf_iterator<char>(&request_buf)), std::istreambuf_iterator<char>() );
    if(DEBUG) std::cout << "Ho letto  " << response_json << std::endl;

    try {
        ControlMessage cm{response_json};
        return cm;}
    catch(std::exception& e){
        std::cerr<<"Error" << e.what()  << std::endl;
    }


}