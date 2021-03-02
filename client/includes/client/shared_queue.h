#pragma once

#include <list>
#include "file_sipper.h"
#include <sender.h>
#define MAX_CONCURRENT_ACTIVE_FS 5
/// This is the shared queue. Each FileSipper is demanded to send a file. The Sender decides who starts sending
class SharedQueue{
    //List of all shared_ptr to FileSippers
    std::list<std::shared_ptr<FileSipper>> fs_list;
    //Protect shared list (fs_list) from being simultaneously accessed by multiple threads.
    std::mutex m;
    //Condition Variable in order to block other threads.
    std::condition_variable cv;
    //Active fileSipper counter (fileSipper assigned to a thread of the pool)
    std::atomic<int> active_fs = 0;
    //Shutdown flag
    std::atomic<bool> flag = true;
    //Singletone pointer
    static SharedQueue* m_SharedQueue;
    SharedQueue()= default;

public:
    SharedQueue(const SharedQueue&)= delete;
    SharedQueue& operator=(const SharedQueue&)=delete;
    static SharedQueue* get_Instance();
    void setFlag(bool flag_value);
    std::shared_ptr<FileSipper> get_ready_FileSipper();
    void remove_element(const std::shared_ptr<FileSipper>& file_sipper);
    void insert(std::shared_ptr<FileSipper> file_sipper); // This one MUST like this, not const reference.

};

