#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/io_service.hpp>
#include <fstream>
#include <queue>
#include <file_sipper.h>
#include <condition_variable>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <shared_queue.h>


/// Takes files from the queue and gives them to FileSippers, in order to be sent
class Sender {

    //TODO: is it ok threads here??
    //std::vector<std::thread> threads[N];
    int count = 0;
    //std::mutex m;
    //std::condition_variable cv;
    bool flag = true;

public:


    void Sender_Action();

    bool isFlag() const;

    void setFlag(bool flag);

    std::shared_ptr<FileSipper> choosen();


};



