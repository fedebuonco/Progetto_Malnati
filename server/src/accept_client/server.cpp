//
// Created by fede on 10/4/20.
//
#include <boost/asio.hpp>
#include <filesystem>
#include "accept_client.h"
#include "server.h"

bool DEBUG=true;

Server::Server(std::filesystem::path serverPath) : stop_(false), serverPath(serverPath){};

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
    AcceptClient acc{ios_, port_num,this->serverPath};
    while(!stop_.load()) {
        acc.SpawnSession(stop_);
    }

}

///Stops the server by waiting for the join of the thread spawned on the Start.
void Server::Stop() {

    stop_.store(true);

    try {
        boost::asio::ip::tcp::socket sock(ios_);
        boost::asio::ip::tcp::endpoint ep_(boost::asio::ip::address::from_string("127.0.0.1"), 3333);
        sock.open(ep_.protocol());
        sock.connect(ep_);

        std::cout<<"Starting to shutdown Synchronous Server..."<<std::endl;
        ControlMessage check_result{5};

        boost::asio::write(sock, boost::asio::buffer(check_result.ToJSON()));
        sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both);

        sock.close();
        // This closes the server by joining the main. It kill also the accept_client.
        // If the accept client has already spawned the last service then
        // the main closes the last service during its switch case.
        // In order to not do that we stop the spawn of the last service by checking the global var.
        thread_->join();
        std::cout<<"Successfully to shutdown Synchronous Server..."<<std::endl;
    } catch (std::exception &e) {
        //The interrupt should occur from another thread
        std::cerr<<"Thread not joined"<<std::endl;
        std::exit(0);
    }

}
