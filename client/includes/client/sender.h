//
// Created by fede on 12/9/20.
//

#ifndef CLIENT_SENDER_H
#define CLIENT_SENDER_H


#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/io_service.hpp>
#include <fstream>
#include <queue>
#include "file_sipper.h"
#include <condition_variable>

#define N 10



/// This is the shared queue. Each FileSipper is demanded to send a file. The Sender decides who starts sending
class Shared_Queue{

public:

    std::queue<FileSipper> queue;
    std::mutex m;

    int size(){
        std::lock_guard<std::mutex> l(m);
        return queue.size();
    }

    //Extract next FileSipper to start. Not remove from the queue!
    const FileSipper& extract(){
        std::lock_guard<std::mutex> l(m);
        const FileSipper& s =  queue.front();
        return s;
    }



    void remove_end(){
        std::lock_guard<std::mutex> l(m);
        queue.pop();
    }

    void Shared_Queue::add(const FileSipper& s){
        std::lock_guard<std::mutex> l(m);
        queue.push(s);
    }



};

/// Takes files from the queue and gives them to FileSippers, in order to be sent
class Sender {

    //TODO: is it ok threads here??
    std::vector<std::thread> threads[N];

    int count = 0;
    std::condition_variable cv;

    std::shared_ptr<Shared_Queue> shared_queue;
public:
    Sender( std::shared_ptr<Shared_Queue> queue): shared_queue(queue){
        //TODO: start threadpool
        //start thread-pool
    };
    void insert(const FileSipper& s);
    void Sender_Action();


private:


};




#endif //CLIENT_SENDER_H
