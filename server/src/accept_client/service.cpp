//
// Created by fede on 10/4/20.
//

#include "service.h"
void Service::ReadRequest() {
    //TODO read and check error
    system::error_code ec;
    boost::asio::async_read(sock_, request_buf_, []);
    if (ec != boost::asio::error::eof){
        //qua se non ho ricevuto la chiusura del client
        std::cout<<"DEBUG: NON ho ricevuto il segnale di chiusura del client";
        throw boost::system::system_error(ec);
    }

}