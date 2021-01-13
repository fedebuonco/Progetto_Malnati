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

    //Sender(std::shared_ptr<SharedQueue> queue): shared_queue(queue){};
    Sender()= default;
    static Sender* m_Sender;

    //TODO: is it ok threads here??
    //std::vector<std::thread> threads[N];
    int count = 0;
    //std::mutex m;
    //std::condition_variable cv;
    bool flag = true;
    std::shared_ptr<SharedQueue> shared_queue;
public:
    void setSharedQueue(const std::shared_ptr<SharedQueue> &sharedQueue);
    //Singleton, eliminate copy and assignment
    Sender(const Sender&)= delete;
    Sender& operator=(const Sender&)=delete;
    static Sender* get_Instance();

    void insert(std::shared_ptr<FileSipper> s);
    void Sender_Action();
    std::shared_ptr<FileSipper> choosen();


};



