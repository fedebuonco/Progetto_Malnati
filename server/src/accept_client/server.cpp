//
// Created by fede on 10/4/20.
//

#include <filesystem>
#include "accept_client.h"
#include "server.h"

Server::Server() : stop_(false){};

/// Starts the server by taking control of the thread_ smart pointer
void Server::Start(unsigned short port_num) {
    //Destroy the current managed object of the thread_ smart pointer
    //useful if we plan to start and stop the server multiple times
    thread_.reset(new std::thread([this,port_num] () {Run(port_num);}));
}

/// Creates the AcceptClient and enters a loop, in the loop we call the AcceptClient SpawnSession until the server is stopped
/// \param port_num Port number where the server should listen
void Server::Run(unsigned short port_num) {
    //TODO esempio di come se usiamo le parentesi tonde qua nel costruttore non funziona
    // malnati ha spiegato come mai a lezione
    AcceptClient acc{ios_, port_num};
    while(!stop_.load()) {
        acc.SpawnSession();
    }
}

///Stops the server by waiting for the join of the thread spawned on the Start.
void Server::Stop() {
    stop_.store(true);
    thread_->join();
}
