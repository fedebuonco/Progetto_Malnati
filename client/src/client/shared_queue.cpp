#include <shared_queue.h>

/***
 * Get the size of the SharedQueue
 * @return
 */
int SharedQueue::size(){
    std::lock_guard<std::mutex> l(m);
    return queue.size();
}

/**
 * Choose a FileSipper from the queue that is ready to send a file.
 * If queue is empty or all FileSipper are already started, Sender will be waiting.
 * @return std::shared_ptr<FileSipper>
 */
std::shared_ptr<FileSipper> SharedQueue::get_ready_FileSipper(){
    std::unique_lock<std::mutex> l(m);

    //While is necessary to prevent spurious wakeups
    //Thread waits when queue is empty OR all files in filesippers are in sending
    while( ( queue.empty() || queue.size() == active_fs )  )
        cv.wait(l, [this]() { return !( queue.empty()  || queue.size() == active_fs ) ; });

    std::shared_ptr<FileSipper> queue_front;

    do{
        queue_front  =  queue.front();
    }while(!queue_front->isReady());

    //I have the mutex
    active_fs ++;

    return queue_front;
}

void SharedQueue::remove_end(){
    std::lock_guard<std::mutex> l(m);
    queue.pop();
}

/***
 * Add a new ptrFileSipper in the SharedQueue
 * @param fsipper
 */
void SharedQueue::add(std::shared_ptr<FileSipper> fsipper){
    std::lock_guard<std::mutex> l(m);
    queue.push(fsipper);
    cv.notify_all();
}

int SharedQueue::getActiveFs(){
    std::lock_guard<std::mutex> l(m);
    return active_fs;
}
