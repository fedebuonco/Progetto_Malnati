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

        acceptor.accept();

        std::cout << "Connesso " << std::endl;
    }
    catch (boost::system::system_error &e) {
            //TODO: ERROR

            return e.code().value();
    }




    return 0;
}

