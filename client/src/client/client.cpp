//
// Created by fede on 10/12/20.
//

#include <sync_tcp_socket.h>
#include "../../includes/client/client.h"
/// Construct a Client.
/// \param re Endpoint to connect to.
Client::Client(RawEndpoint re) {

    this->server_re_ = re;
    //TODO Istanziare una table dei socket attivi? BOh

}

/// Authenticates the client by creating a syncTCPSocket and calling it's auth method.
/// \return
bool Client::Auth() {
    //TODO valutare se mettere try catch

    //SyncTCPSocket for auth using raw endpoint provided at construction.
    SyncTCPSocket client_sync(server_re_.raw_ip_address, server_re_.port_num);
    //socket will retry 5 times to connect
    client_sync.ConnectServer(5);

    if(! Config::get_Instance()->isConfig() ){
        Config::get_Instance()->startConfig();
    }
    //TODO Lanciare detro isConfig e startConfig delle eccezioni di una classe
    // da fare e poi catcharle nel main per terminare il programma.
    // Es. No indirizzo ip per connessione server

    //Auth loop, while the password is wrong asks for a new identity. Will exit as soon as
    //data inserted is verified correctly.
    while( !client_sync.Authenticate() ){
        Config::get_Instance()->startConfig();
    }

    return true;

}

