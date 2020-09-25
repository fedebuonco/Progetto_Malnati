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

        boost::asio::streambuf buf2;
        boost::asio::read_until(sock, buf2, '\n');

        std::string message;

        std::istream input_stream(&buf2);
        std::getline(input_stream, message);


        /*int num = 9;

        unsigned char MESSAGE_SIZE = num;

        char buf[MESSAGE_SIZE];

        boost::asio::read(sock, boost::asio::buffer(buf, MESSAGE_SIZE));

        std::string read2_ = std::string(buf, MESSAGE_SIZE);*/

        std::cout << "Ho letto  " << message << std::endl;

        std::string uguale="0";

        if(message=="MARCO MARCO"){
            uguale="1";
        }

        boost::asio::write(sock, boost::asio::buffer(uguale));
        std::cout << "Size: " << uguale.size();

       //TODO fare ciclo qui si chiude il server
         }
    catch (boost::system::system_error &e) {
            //TODO: ERROR
            std::cerr << "Errore genertico server " << std::endl;
            return e.code().value();
    }




    return 0;
}

