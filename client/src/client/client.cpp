#include <sync_tcp_socket.h>
#include <control_message.h>
#include <authentication.h>
#include <client.h>
#include <filesystem>
#include <iostream>
#include <tree_t.h>
#include <patch.h>

//CryptoPP
#include <files.h>
#include <sha.h>
#include <hex.h>

#include <database.h>
#include <boost/algorithm/string/predicate.hpp>
#include <utility>

void ClearScreen()
{
    std::cout << std::string( 100, '\n' );
}

/// Construct a Client, execute the first hashing of each file and puts it in a state ready to track any changes in the folder.
/// \param re :  Endpoint to connect to.
/// \param folder_watched : folder path that we want to keep monitored.
Client::Client(RawEndpoint re, const std::filesystem::path& folder_watched) :
    server_re_(std::move(re)),
    folder_watched_(folder_watched),
    db_file_(folder_watched / ".hash.db"),
    watcher_(db_file_, folder_watched_)
{
    if(!Auth()){
        std::cerr << "Username and/or password are not correct" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // We recover the sending files
    int recovered = RecoverSending();
    std::cout << "\nRecovered " << recovered << " files from previous failed sending" << std::endl;


    // We start watching for further changes
    StartWatching();
}

/// We set the callback and ask to the watcher to start tracking any changes on the watched folder.
/// We bind to the callback this, to that when called from the watcher, "this" is defined.
/// https://stackoverflow.com/questions/14189440/c-callback-using-class-member/14189561
void Client::StartWatching(){
    Syncro();
    watcher_.SetUpdateCallback([this] { Syncro(); });
    watcher_.Start(folder_watched_);

}

/// This is the main function, create a Tree and asks for the server Tree, with those it creates a patch and process it
/// The processed patch is then used to fuel the SendPatch()
void Client::Syncro(){
    // We perform the InitHash that helps us not hashing everything each time
    InitHash();

    // Generate Client tree & time starting from the folder watched
    TreeT client_treet(this->folder_watched_);

    // We ask the server to send us the user TREE & TIME.
    TreeT server_treet = RequestTree();

    // Need to figure out which files the server doesn't have (we need to send) and the file the server have to delete.
    Patch update(client_treet, server_treet);

    // We align the DB updating the status
    DatabaseConnection db{db_file_, folder_watched_};
    db.AlignStatus(update.same_file_same_lmt_vector);

    // Send to the server the list of file to remove
    SendRemoval(update);

    // This function identify the file that have to be sent really
    update.Dispatch(db_file_, folder_watched_);

    if (DEBUG) {
        ClearScreen();
        update.PrettyPrint();
    }

}

/// Will send username and password. After that it will shutdown the send part of the socket thus providing the server a way to
/// tell that the connection is over. After the authentication the socket is basically useless and needs to be shutdown completely.
/// \return 'True' if Auth went ok and user has successfully logged in, 'False' if not.
bool Client::Auth() {

    //SyncTCPSocket for auth using raw endpoint provided at construction.
    SyncTCPSocket tcpSocket(server_re_.raw_ip_address, server_re_.port_num);
    //socket will retry 5 times to connect
    tcpSocket.ConnectServer(5);

    //Read the credential to put inside the control message
    Credential credential = Authentication::get_Instance()->ReadCredential();

    //Creation of the message with type:1 (it means an authentication request) with inside 'Username' and 'Password'
    ControlMessage auth_message{1};
    auth_message.AddElement("Username", credential.username_);
    auth_message.AddElement("HashPassword", credential.hash_password_);

    //And sending it formatted in JSON language
    SyncWriteCM(tcpSocket, auth_message);

    //Now we have sent the ControlMessage, so from now on we will wait for the response.

    //Now we use a message control for the response
    ControlMessage response_message = SyncReadCM(tcpSocket);

    //We are expecting a message with an auth response (type:51)
    if(response_message.type_ == 51){
        //The answer is inherent with the request, so we read the reply message.

        //We retrieve the auth information inside the response message.
        std::string auth_response = response_message.GetElement("auth");

        if(auth_response=="true") {
            //The user is authenticated so we return true
            if(DEBUG) std::cout << "\nUser \"" << Config::get_Instance()->ReadProperty("username") << "\" successfully authenticated." << std::endl;
            return true;
        }
    }

    //If we don't enter to return true, there is a problem: the authentication failed or the reply message is wrong.
    //In any case, we return false.
    return false;
}

/**
 * Scans the db and put the failed "SENDING" to "NEW" again, they will be re-sync as soon as possible.
 * @return  Returns the recovered files.
 */
int Client::RecoverSending() {

    // We open the db once here so that we limit the overhead
    DatabaseConnection db(db_file_,folder_watched_);

    // We change the sending to new
    int recovered = db.SetBackToNew();

    return recovered;
}


/// Request current tree and times of the cloud dir stored in the server.
/// Handles the result by starting the diff computation.
TreeT Client::RequestTree() {
    //SyncTCPSocket for request
    Credential credential = Authentication::get_Instance()->ReadCredential();
    SyncTCPSocket tcpSocket(server_re_.raw_ip_address, server_re_.port_num);
    //socket will retry 5 times to connect
    tcpSocket.ConnectServer(5);

    // We create the message for TreeRequest (Type: 2) adding inside 'Username' and 'HashPassword'
    ControlMessage message_obj{2};
    message_obj.AddElement("Username", credential.username_);
    message_obj.AddElement("HashPassword", credential.hash_password_);

    //And sending it formatted in JSON language
    SyncWriteCM(tcpSocket, message_obj);

    //Now we have sent the ControlMessage, so from now on we will wait for the response.

    //Now we use a message control for the response
    ControlMessage response_message = SyncReadCM(tcpSocket);

    //We get the tree & time list
    std::string tree = response_message.GetElement("Tree");
    std::string time = response_message.GetElement("Time");

    //Build a TreeT object with the server tree
    TreeT result{tree,time};

    return result;
}

///
void Client::SyncWriteCM(SyncTCPSocket& stcp, ControlMessage& cm){

    boost::system::error_code ec;
    boost::asio::write(stcp.sock_, boost::asio::buffer(cm.ToJSON()), ec);
    if (ec){
        if(DEBUG) std::cerr<<"Error writing Control Message; message: " << ec.message() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    //We write and close the send part of the SyncTCPSocket, in order to tell the server that we have finished writing
    stcp.sock_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    if (ec){
        if(DEBUG) std::cerr<<"Error writing Control Message; message: " << ec.message() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

/// We read until the eof, then we return a ControlMessage using the buffer we read
ControlMessage Client::SyncReadCM(SyncTCPSocket& stcp){

    boost::asio::streambuf response_buf;

    boost::system::error_code ec;
    boost::asio::read(stcp.sock_, response_buf, ec);

    // This checks if the client has finished writing
    if (ec != boost::asio::error::eof){

        if(DEBUG) std::cerr<<"The server is not responding, try later. Message " << ec.message() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    //Read the response_buf using an iterator and store it in a string in order to store it in a ControlMessage
    std::string response_json((std::istreambuf_iterator<char>(&response_buf)), std::istreambuf_iterator<char>() );

    ControlMessage cm{response_json};

    //We are expecting a message with an auth response (type:51)
    if(cm.type_ == 51) {
        //The answer is inherent with the request, so we read the reply message.

        //We retrieve the auth information inside the response message.
        std::string auth_response = cm.GetElement("auth");

        if (auth_response == "false") {
            //The user is authenticated so we return true
            if (DEBUG)
                std::cout << "\nUser \"" << Config::get_Instance()->ReadProperty("username")
                          << "\" successfully authenticated." << std::endl;
        }
    }

    return cm;
}

/**
 * It generate a list of file formatted by /n; this will be sent to the server
 * @param vector : vector with file to be deleted
 * @return string properly formatted to put inside a control message
 */
std::string genTree(const std::vector<std::string>& vector) {
    std::string tree;
    for(const auto& element : vector) {
        tree.append(element + "\n");
    }
    return tree;
}


/// Here we firstly send a ControlMessage that will tell what to delete in the server. After that we will start sending
/// the newer files.
/// \param update processed patch
void Client::SendRemoval(Patch& update){

    //If the list is empty we don't send the list of 'to be deleted' to the server
    if(update.removed_.empty())  return;

    //The list is not empty, so we have some file to send. We prepare the message type:3

    Credential credential_ = Authentication::get_Instance()->ReadCredential();

    //Creation of the Auth ControlMessage type: 3 with inside 'Username', 'Password' and the list of 'file to be deleted'
    ControlMessage delete_message{3};
    delete_message.AddElement("Username",credential_.username_);
    delete_message.AddElement("HashPassword",credential_.hash_password_ );
    delete_message.AddElement("To_be_deleted", genTree(update.removed_));

    //And sending it formatted in JSON language
    SyncTCPSocket tcpSocket(server_re_.raw_ip_address, server_re_.port_num);
    tcpSocket.ConnectServer(5);
    SyncWriteCM(tcpSocket, delete_message);
}


/// Test each file to see if already present in the hash db, and acts accordingly in order to keep a database of each
/// hash performed.
void Client::InitHash(){
    int n_attempts = 10;
    while(n_attempts > 0) {
        try {
            // We open the db once here so that we limit the overhead
            DatabaseConnection db(db_file_, folder_watched_);

            // For each file in the folder we look in the db using the relative filename and the last modified time.
            for (auto itEntry = std::filesystem::recursive_directory_iterator(folder_watched_,
                                                                              std::filesystem::directory_options::skip_permission_denied);
                 itEntry != std::filesystem::recursive_directory_iterator();
                 ++itEntry) {

                // We take the current element path, make it relative to the path specified and then we make it
                // in a cross platform format (cross_platform_relative_element_path = cross_platform_rep)
                auto element_path = itEntry->path();
                std::filesystem::path relative_element_path = element_path.lexically_relative(folder_watched_);
                std::string cross_platform_rep = relative_element_path.generic_string();

                // We also add the "/" if it is a directory in order to diff it from non extension files.
                if (std::filesystem::is_directory(element_path))
                    cross_platform_rep += "/";

                // Now if the current element is a dir or the db or its a temporary file we can go to the next iteration
                if (std::filesystem::is_directory(element_path) || cross_platform_rep == ".hash.db" ||
                    boost::algorithm::ends_with(cross_platform_rep, "~"))
                    continue;

                // We now need to retrieve the last modified time.
                struct stat temp_stat;
                // Put inside our struct temp_state the metadata like last modified time
                stat(element_path.generic_string().c_str(), &temp_stat);
                unsigned long mod_time = temp_stat.st_mtime;

                // Now we have the tuple ( cross_platform_rep , mod_time )
                // We use this tuple to see if we already hashed that version of the file.
                // If we had, then we take the hash from the db, without hashing a second time the same file.

                if (!db.AlreadyHashed(cross_platform_rep, std::to_string(mod_time))) {
                    //Here only if the tuple ( cross_platform_rep , mod_time ) is not present inside DB, so we need to hash and then update the db.

                    //Hash the file and return the digest to insert in the DB
                    std::string digest = HashFile(element_path);
                    if(!digest.empty()){
                        //We have the hash of the file and we can insert it in the DB
                        db.InsertDB(cross_platform_rep, digest, std::to_string(mod_time));
                    }
                }
            }

            // Now we clean the db for files that are not anymore in the folder.
            db.CleanOldRows();

            // We now have the DB and client folder aligned.
            return;
        }
        catch (std::filesystem::filesystem_error &e) {
            std::cerr << "Error Filesystem (InitHash): " << n_attempts << e.what() << std::endl;
            n_attempts--;

            if(n_attempts==0){
                std::exit(EXIT_FAILURE);
            }

        }
        catch (std::exception &e) {
            std::cerr << "Error generic (InitHash) " << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
}

std::string Client::HashFile(const std::filesystem::path& element_path) {
    CryptoPP::SHA256 hash;
    std::string digest;

    try {
        CryptoPP::FileSource f(
                element_path.generic_string().c_str(),
                true,
                new CryptoPP::HashFilter(hash,
                                         new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));
    }
    catch (CryptoPP::FileStore::Err &e) {
        //Here because CryptoPP can't open the file because we are copying the file into the folder
        //Another possibility is that the file doesn't exist anymore.
        //In both ways we recover from this exception.
    }
    catch (CryptoPP::Exception &e) {
        std::cout << "Error CryptoPP (HashFile) " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    catch (std::exception &e) {
        std::cout << "Error generic (HashFile) " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }


    return digest;
}
