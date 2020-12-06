#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <control_message.h>
#include <filesystem>
#include <tree_t.h>
#include "../../includes/database/database.h"
#include "service.h"

/// Starts handling the particular client that requested a service. Spawns a thread that actually handle the request and detach it
/// \param sock TCP socket conneted to the client
void Service::ReadRequest(std::shared_ptr<asio::ip::tcp::socket> sock) {
    std::thread th(([this, sock] () {HandleClient(sock);}));
    th.detach();
}

/// Real handling starts here. Should distinguish auth requests and tree requests
void Service::HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock) {

    // Here we read the request
    // parse it in a ptree,
    // use the ptree to build the ControlMessage
    // we then check that the message is authenitic
    // and then we switch case in order to correctly handle it.

    boost::asio::streambuf request_buf;
    boost::system::error_code ec;
    boost::asio::read(*sock, request_buf, ec);
    // This checks if the client has finished writing
    if (ec != boost::asio::error::eof){
        //qua se non ho ricevuto la chiusura del client
        std::cout<<"DEBUG: NON ho ricevuto il segnale di chiusura del client";
        throw boost::system::system_error(ec);
    }

    //Read the request_buf using an iterator and store it in a string
    //TODO might be an easier method to do this
    std::string request_json( (std::istreambuf_iterator<char>(&request_buf)), std::istreambuf_iterator<char>() );
    //DEBUG
    std::cout << "Ho letto  " << request_json << std::endl;

    //Now we parsed the request and we use the ptree object in  order to create the corresponding ControlMessage
    ControlMessage request_message{request_json};

    // Here we check that the message is authentic
    if (!CheckAuthenticity(request_message)){
        ControlMessage check_result{51};
        check_result.AddElement("auth", "false");
        boost::asio::write(*sock, boost::asio::buffer(check_result.ToJSON()));
        // Send the eof error shutting down the server.
        // TODO qua magicamente va ignorato l'errore GRAVISSIMO
        sock->shutdown(boost::asio::socket_base::shutdown_both, ec);
    }

    // Here based on the type of the message we switch accordingly.
    // All the message that arrive here are authentic.
    switch (request_message.type_) {
        case 1:{//AUTH REQUEST
            ControlMessage check_result{51};
            check_result.AddElement("auth", "true");
            boost::asio::write(*sock, boost::asio::buffer(check_result.ToJSON()));
            // Send the eof error shutting down the server.
            // TODO qua magicamente va ignorato l'errore GRAVISSIMO
            sock->shutdown(boost::asio::socket_base::shutdown_both, ec);
            break;
        }
        case 2:{//TREE & TIME Request
            //Let's start building the Response Control Message

            ControlMessage treet_result{52};
            // We compute & add the tree
            std::string username = request_message.GetElement("Username");

            Database db;
            std::string user_folder_name = db.getUserPath(username);

            std::cout << "User folder: " << user_folder_name << std::endl;

            std::filesystem::directory_entry users_tree{ "../backupFiles/usersTREE"};
            if (!users_tree.exists()) {
                //User doesn't have a folder, so we create a new one and we add a user db

                //TODO Check error during creation of directory
                std::filesystem::create_directories("../backupFiles/usersTREE");
            }


            std::filesystem::directory_entry user_directory_path{ "../backupFiles/backupROOT/"+user_folder_name};


            //Check if this user has a folder inside backupROOT
            if (!user_directory_path.exists()) {
               //User doesn't have a folder, so we create a new one and we add a user db

               //TODO Check error during creation of directory
               std::filesystem::create_directories(user_directory_path);
               std::cout << "User doesn't have the folder. I create a new folder name: " << user_folder_name << std::endl;


               //Create DB file for the user TREE
               db.createTable(user_folder_name);
            }
            /*TODO Se si verifica un eccezione tra la creazione dello userfolder e il createTable (db utente)
            vediamo la directory ma non vediamo il DB. Azione da fare è RIPROVA. Questo potrebbe creare problemi, consiglio di fare nella catch una politica
             che cancella sia la cartella principale (tanto in questo punto è vuota) ed il db. Ricordo comunque che nel caso in cui
             il db esiste quando entriamo sulla createTable droppo comunque la tabella e la ricreo.
            */

            //TODO - IL PROGRAMMA TERMINA MALE SE METTI CARTELLA CHE NON TROVA
            // TODO change the dir accordingly to username of the client
            TreeT server_treet (user_directory_path);
            treet_result.AddElement("Tree", server_treet.genTree());
            treet_result.AddElement("Time", server_treet.genTimes());
            //TODO Implement DB and retrieve time according to this function
            boost::asio::write(*sock, boost::asio::buffer(treet_result.ToJSON()));
            // Send the eof error shutting down the server.
            // TODO qua magicamente va ignorato l'errore GRAVISSIMO
            sock->shutdown(boost::asio::socket_base::shutdown_both, ec);
            break;
        }
        case 3:{//DELETE REQUEST
            //TODO Implement DB and retrive the dir accordingly to username of the client
            // Here we delete the files that are in the mess.
            break;
        }
    }

    // Now the service class was instantiated in the heap &
    // As the service class has finished it's work we are gonna delete it here
    // "delete this" is known to be bad code, but if we follow some security
    // advices we can use it.
    delete this;
}

bool Service::CheckAuthenticity(const ControlMessage& cm){
    Database authentication;
    return authentication.auth(cm.username_, cm.hashkey_);
}