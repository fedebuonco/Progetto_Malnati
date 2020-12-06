#include <sync_tcp_socket.h>
#include <control_message.h>
#include <authentication.h>
#include <client.h>
#include <filesystem>
#include <iostream>
#include <tree_t.h>
#include <patch.h>

/// Construct a Client and puts it in a state ready to track any changes in the folder.
/// \param re Endpoint to connect to.
/// \param folder_watched folder path that we want to keep monitored.
Client::Client(RawEndpoint re, std::filesystem::path folder_watched) {
    this->server_re_ = re;
    this->folder_watched_ = folder_watched;
    if(!Auth()){
        std::cerr << "Username and/or password are not correct" << std::endl;
        std::exit(12);
    }
    StartWatching();
}

/// We set the callback and ask to the watcher to start tracking any changes on the watched folder.
/// We bind to the callback this, to that when called from the watcher, "this" is defined.
/// https://stackoverflow.com/questions/14189440/c-callback-using-class-member/14189561
void Client::StartWatching(){
    Syncro();
    watcher_.SetUpdateCallback(std::bind(&Client::Syncro, this));
    watcher_.Start(folder_watched_);
}

/// This is the main function, create a Tree and asks for the server Tree, with those it cretes a patch and process it
/// The processed patch is then used to fuel the SendPatch()
void Client::Syncro(){
    //Generate Client tree string
    // std::string client_tree;
    // client_tree = GenerateTree(this->folder_watched_);
    TreeT client_treet(this->folder_watched_);
    // Then we ask for the Server's TreeT
    TreeT server_treet = RequestTree();
    Patch update(client_treet, server_treet);
    if (DEBUG)
        update.PrettyPrint();
    //SendPatch(update);

    }

/// Will send username and password. After that it will shutdown the send part of the socket thus providing the server a way to
/// tell that the connection is over. After the authenticate the socket is basically useless and needs to be shutdown completly.
/// \return True if Auth went ok and user has successfully logged in, False if not.
bool Client::Auth() {
    //TODO qualche eccezione da fare il catch?
    //SyncTCPSocket for auth using raw endpoint provided at construction.
    SyncTCPSocket tcpSocket(server_re_.raw_ip_address, server_re_.port_num);
    //socket will retry 5 times to connect
    tcpSocket.ConnectServer(5);
    //Try to authenticate and return true / false

    Credential credential = Authentication::get_Instance()->ReadCredential();

    //Creation of the Auth ControlMessage type = 1
    ControlMessage auth_message{1};

    //Adding User and Password
    auth_message.AddElement("Username", credential.username_);
    auth_message.AddElement("HashPassword", credential.hash_password_);

    //And sending it formatted in JSON language
    SyncWriteCM(tcpSocket, auth_message);

    //Now we have sent the ControlMessage, so from now on we will wait for the response.

    //Now we use a message control for the response
    ControlMessage response_message = SyncReadCM(tcpSocket);

    if(response_message.type_ == 51){// it means we are actually dealing with a auth response (what we were expecting)
        //TODO return true false accordingly to the body of the control message received for now we just check it is an auth response
        // later on we will check the result
        // auth = true /false
        std::string auth_response = response_message.GetElement("auth");

        if(auth_response=="true") {

            if(DEBUG) std::cout << "\n\nUser " << Config::get_Instance()->ReadProperty("username") << " successfully authenticated."
                                << std::endl;
            return true;
        }

    }

    //If we don't go to return true, there is a problem so return false
    return false;
}

/// Request current tree and times of the cloud dir stored in the server. Handles the result by starting the diff
/// computation.
TreeT Client::RequestTree() {
    //SyncTCPSocket for request
    Credential credential = Authentication::get_Instance()->ReadCredential();
    SyncTCPSocket tcpSocket(server_re_.raw_ip_address, server_re_.port_num);
    //socket will retry 5 times to connect
    tcpSocket.ConnectServer(5);
    //Here we create the ControlMessage for TreeRequest ( Type = 2 )
    ControlMessage message_obj{2};
    //Adding Username and Hash Password
    message_obj.AddElement("Username", credential.username_);
    message_obj.AddElement("HashPassword", credential.hash_password_);
    //And sending it formatted in JSON language
    //And sending it formatted in JSON language
    SyncWriteCM(tcpSocket, message_obj);

    //Now we have sent the ControlMessage, so from now on we will wait for the response.

    //Now we use a message control for the response
    ControlMessage response_message = SyncReadCM(tcpSocket);
    if(DEBUG) std::cout << "Tree received successfully" << std::endl;
    //We get the tree
    std::string tree = response_message.GetElement("Tree");
    //TODO Uncomment this as soon as the server will send the times
    std::string time = response_message.GetElement("Time");
    TreeT result{tree,time};
    return result;
}

bool Client::SyncWriteCM(SyncTCPSocket& stcp, ControlMessage& cm){
    //We write and close the send part of the SyncTCPSocket, in order to tell the server that we have finished writing
    boost::asio::write(stcp.sock_, boost::asio::buffer(cm.ToJSON()));
    stcp.sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
}

ControlMessage Client::SyncReadCM(SyncTCPSocket& stcp){
    // We read until the eof, then we return a ControlMessage using the buffer we read.
    boost::asio::streambuf response_buf;
    boost::system::error_code ec;
    boost::asio::read(stcp.sock_, response_buf, ec);
    // This checks if the client has finished writing
    if (ec != boost::asio::error::eof){
        //TODO Sometimes we get here. When the server shuts down
        if(DEBUG) std::cerr<<"DEBUG: NON ho ricevuto il segnale di chiusura del client";
        throw boost::system::system_error(ec);
    }
    //Read the response_buf using an iterator and store it in a string In order to store it in a ControlMessage
    // TODO might be an easier method to do this
    std::string response_json((std::istreambuf_iterator<char>(&response_buf)), std::istreambuf_iterator<char>() );
    ControlMessage cm{response_json};
    return cm;
}

/// Here we firstly send a ControlMessage that will tell what to delete in the server. After that we will start sending
/// the newer files.
/// \param update processed patch
void Client::SendPatch(Patch& update){
    //Sending delete ControlMessage
    //Creation of the Auth ControlMessage type = 3
    Credential credential_ = Authentication::get_Instance()->ReadCredential();
    SyncTCPSocket tcpSocket(server_re_.raw_ip_address, server_re_.port_num);
    ControlMessage delete_message{3};
    //Adding User and Password
    delete_message.AddElement("Username",credential_.username_);
    delete_message.AddElement("Password:",credential_.hash_password_ );
    //We add the delete string
    //delete_message.AddElement("To_be_deleted",update.to_be_deleted_);
    //And sending it formatted in JSON language
    boost::asio::write(tcpSocket.sock_, boost::asio::buffer(delete_message.ToJSON()));
    // we sent the Auth message, we will shutdown in order to tell the server that we sent all
    tcpSocket.sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

    // Now we can focus on new and common
    //TODO Here send the files asyncronulsy

}