#include <iostream>
#include <boost/asio.hpp>

int main(){

    const int BACKLOG_SIZE = 10;

    unsigned short port_num = 3333;

    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), port_num);

    boost::asio::io_service ios;

    try {
        boost::asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

        acceptor.bind(ep);

        acceptor.listen(BACKLOG_SIZE);

        boost::asio::ip::tcp::socket sock(ios);

        acceptor.accept(sock);



/*
        char bufSize[1];

        boost::asio::read(sock, boost::asio::buffer(bufSize, 1));

        std::cout << "Mess" << bufSize << std::endl;
*/

        boost::asio::streambuf request_buf;

        boost::system::error_code ec;

        //Recive the req
        boost::asio::read(sock, request_buf, ec);
        if (ec != boost::asio::error::eof){
            //qua se non ho ricevuto la chiusura del client
            std::cout<<"DEBUG: NON ho ricevuto il segnale di chiusura del client";
            throw boost::system::system_error(ec);
        }

        std::string message;
        std::istream input_stream(&request_buf);
        std::getline(input_stream, message);

        std::cout << "Ho letto  " << message << std::endl;

        std::string uguale="0";

        if(message=="MARCO MARCO"){
            uguale="1";
        }

        boost::asio::write(sock, boost::asio::buffer(uguale));
        // Send the eof error shutting down the server.
        sock.shutdown(boost::asio::socket_base::shutdown_send);
       //TODO fare ciclo qui si chiude il server
         }
    catch (boost::system::system_error &e) {
            //TODO: ERROR
            std::cerr << "Errore genertico server " << std::endl;
            return e.code().value();
    }




    return 0;
}

