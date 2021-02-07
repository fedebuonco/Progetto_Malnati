#include <boost/asio.hpp>
#include <filesystem>
#include <memory>
#include <utility>
#include <accept_client.h>
#include <server.h>

bool DEBUG=true;

/**
 * Constructor.
 * @param serverPath : path of the server folder
 */
Server::Server(std::filesystem::path serverPath) : stop_(false), serverPath(std::move(serverPath)){}

/**
 * Starts the server by taking control of the thread_ smart pointer
 * @param port_num of the server
 */
void Server::Start(unsigned short port_num) {

    //Run the server and save it in a thread_ smart pointer.
    //Useful if we plan to start and stop the server multiple times
    thread_ = std::make_unique<std::thread>( [this, port_num] () {
            Run(port_num);
    });

}

/**
 * Creates the AcceptClient and enters a loop, in the loop we call the AcceptClient SpawnSession until the server is stopped
 * @param port_num Port number where the server should listen
 */
void Server::Run(unsigned short port_num) {

    //Create an AcceptClient to handle client request
    AcceptClient acc{ios_, port_num,this->serverPath};

    //We continue to accept requests from clients until flag is false (i.e. until we close the server)
    while(!stop_.load()) {
        acc.SpawnSession(stop_);
    }

}

///Stops the server by waiting for the join of the thread spawned on the Start.
void Server::Stop() {
    //Set the flag to true because we are shutting down the server
    stop_.store(true);

    try {
        // We send a fake message in order to close the server.

        // We have a race condition where the last service handling this message could or could not be spawned
        // so we send it but an exception telling us that the service is not present could arise.
        // In order to manage that we just save the exception in ec, without ever looking at it.
        boost::system::error_code ec;
        boost::asio::ip::tcp::socket sock(ios_);
        boost::asio::ip::tcp::endpoint ep_(boost::asio::ip::address::from_string("127.0.0.1"), 3333);
        sock.open(ep_.protocol(), ec);
        sock.connect(ep_,ec);

        if(ec){
            std::cerr << "Ecc:" << ec << std::endl;     //TODO: I never print the error so we can keep it
        }

        //std::cout << "Starting to shutdown Synchronous Server..." << std::endl;
        ControlMessage check_result{5};

        boost::asio::write(sock, boost::asio::buffer(check_result.ToJSON()),ec);
        sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);

        if(ec){
            std::cerr << "Ecc:" << ec << std::endl;     //TODO: I never print the error so we can keep it
        }

        sock.close();

        // This closes the server by joining the main. It kill also the accept_client.
        // If the accept client has already spawned the last service then
        // the main closes the last service during its switch case.
        // In order to not do that we stop the spawn of the last service by checking the global var.

        thread_->join();
        //std::cout<<"Successfully to shutdown Synchronous Server..."<<std::endl;
    }
    catch (std::exception &e) {
        //The interrupt should occur from another thread
        std::cerr<<"Thread not joined "<<  e.what() << std::endl;
        std::exit(0);
    }

}
