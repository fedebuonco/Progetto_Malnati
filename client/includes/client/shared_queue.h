#pragma once


#include <list>
#include "file_sipper.h"
#include <sender.h>

/// This is the shared queue. Each FileSipper is demanded to send a file. The Sender decides who starts sending
class SharedQueue{

    std::list<std::shared_ptr<FileSipper>> fs_list;

    std::mutex m;
    std::condition_variable cv;

    std::atomic<int> active_fs = 0;

    std::atomic<bool> flag = true;

    static SharedQueue* m_SharedQueue;
    SharedQueue()= default;

public:
    SharedQueue(const SharedQueue&)= delete;
    SharedQueue& operator=(const SharedQueue&)=delete;
    static SharedQueue* get_Instance();

    void setFlag(bool flag_value);

    int list_size();

    std::shared_ptr<FileSipper> get_ready_FileSipper();

    void remove_element(const std::shared_ptr<FileSipper>& file_sipper);
    void insert(const std::shared_ptr<FileSipper>& file_sipper);

};

