#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/io_service.hpp>
#include <fstream>
#include <condition_variable>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <shared_queue.h>
#include <file_sipper.h>


/// Takes files from the queue and gives them to FileSippers, in order to be sent
class Sender {
    //TODO: tUTTI E TRE SI POSSONO ELIMINARE @marco
    //int count = 0;
    //std::mutex m;
    //std::condition_variable cv;

    bool flag = true;

public:

    void Sender_Action() const;
    void setFlag(bool flag);
};



