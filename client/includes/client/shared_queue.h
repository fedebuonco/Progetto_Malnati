#pragma once


#include <queue>
#include "file_sipper.h"
#include <sender.h>

/// This is the shared queue. Each FileSipper is demanded to send a file. The Sender decides who starts sending
class SharedQueue{

    std::queue<std::shared_ptr<FileSipper>> queue;
    std::mutex m;
    std::condition_variable cv;
    //possibly use atomic
    int active_fs = 0;
    static SharedQueue* m_SharedQueue;
    bool flag = true;


    //Sender(std::shared_ptr<SharedQueue> queue): shared_queue(queue){};
    SharedQueue()= default;

public:
    void setFlag(bool flag);
    bool isFlag() const;

    int getActiveFs();
    int size();
    //Extract next FileSipper to start. Not remove from the queue!
    std::shared_ptr<FileSipper> get_ready_FileSipper();
    void remove_end();
    void insert(std::shared_ptr<FileSipper> fsipper);

    SharedQueue(const SharedQueue&)= delete;
    SharedQueue& operator=(const SharedQueue&)=delete;
    static SharedQueue* get_Instance();


};

