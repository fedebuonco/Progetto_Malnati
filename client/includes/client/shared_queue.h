#pragma once


#include <queue>
#include "file_sipper.h"

/// This is the shared queue. Each FileSipper is demanded to send a file. The Sender decides who starts sending
class SharedQueue{

private:
    std::queue<std::shared_ptr<FileSipper>> queue;
    std::mutex m;
    std::condition_variable cv;
    //possibly use atomic
    int active_fs = 0;

public:
    int getActiveFs();
    int size();
    //Extract next FileSipper to start. Not remove from the queue!
    std::shared_ptr<FileSipper> get_ready_FileSipper();
    void remove_end();
    void add(std::shared_ptr<FileSipper> fsipper);


};

