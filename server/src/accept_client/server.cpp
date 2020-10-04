//
// Created by fede on 10/4/20.
//

#include "accept_client.h"
#include "server.h"

Server::Server() : stop_(false){};

void Server::Start(unsigned short port_num) {
    thread_.reset(new std::thread([this,port_num] () {Run(port_num);}));
}


void Server::Run(unsigned short port_num) {
    //TODO esempio di come se usiamo le parentesi tonde qua nel costruttore non funziona
    // malnati ha spiegato come mai a lezione
    AcceptClient acc{ios_, port_num};
    while(!stop_.load()) {
        acc.SpawnSession();
    }
}

void Server::Stop() {
    stop_.store(true);
    thread_->join();
}
