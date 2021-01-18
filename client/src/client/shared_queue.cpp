#include <shared_queue.h>

#include <boost/thread/thread.hpp>

SharedQueue *SharedQueue::m_SharedQueue = nullptr;

SharedQueue *SharedQueue::get_Instance() {
    if(!m_SharedQueue){
        m_SharedQueue = new SharedQueue();
    }
    return m_SharedQueue;
}

/***
 * Get the size of the SharedQueue
 * @return
 */
int SharedQueue::size(){
    std::lock_guard<std::mutex> l(m);
    return fs_list.size();
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
    while( ( fs_list.empty() || fs_list.size() == active_fs) && SharedQueue::get_Instance()->isFlag()  ) {
        std::cout<< "EMPTY SHARED QUEUE "<<fs_list.size() << std::endl;
        cv.wait(l, [this]() { return !( (fs_list.empty() || fs_list.size() == active_fs) && SharedQueue::get_Instance()->isFlag() ); });
        std::cout << std::this_thread::get_id << "  pool "<<std::endl;
    }

    if( !SharedQueue::get_Instance()->isFlag() ){
        return nullptr;
    }


    std::shared_ptr<FileSipper> fs_list_front;

    do{
        fs_list_front  =  fs_list.front();
    }while(!fs_list_front->isReady());

    //I have the mutex
    active_fs ++;

    return fs_list_front;
}

//TODO è necessaria?
void SharedQueue::remove_end(){
    std::lock_guard<std::mutex> l(m);
    //fs_list.remove();
}

/***
 * Add a new ptrFileSipper in the SharedQueue
 * @param fsipper
 */
void SharedQueue::insert(std::shared_ptr<FileSipper> fsipper){
    std::lock_guard<std::mutex> l(m);
    fs_list.push_front(fsipper);
    cv.notify_all();
}

int SharedQueue::getActiveFs(){
    std::lock_guard<std::mutex> l(m);
    return active_fs;
}

void SharedQueue::setFlag(bool flag) {
    std::lock_guard<std::mutex> l(m);
    SharedQueue::flag = flag;
    cv.notify_all();
}

bool SharedQueue::isFlag() const {
    return flag;
}
