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
    //Thread waits when queue is empty OR all files in filesippers are in sending.
    //Check Shared Queue status flag.
    while( ( fs_list.size()==0 || (fs_list.size() == active_fs.load())) && SharedQueue::get_Instance()->isFlag()  ) {
        cv.wait(l, [this]() { return !( (fs_list.size()==0 || (fs_list.size() == active_fs.load())) && SharedQueue::get_Instance()->isFlag() ); });
    }

    if( !SharedQueue::get_Instance()->isFlag() ){
        return nullptr;
    }

    //Search for next ready FileSipper.
    std::list<std::shared_ptr<FileSipper>>::iterator it;
    for (it = fs_list.begin(); it != fs_list.end(); ++it){
        if(it->get()->status.load() == false) {
            it->get()->status.store(true);
            active_fs.fetch_add(1);
            return *it;
        }
    }

   return nullptr;

}

/***
 * Remove the selected filesipper from the shared queue.
 * @param fsipper
 */
void SharedQueue::remove_element(std::shared_ptr<FileSipper> fsipper){
    std::lock_guard<std::mutex> l(m);
    fs_list.remove(fsipper);
    SharedQueue::get_Instance()->active_fs.fetch_sub(1);
    if(fs_list.size() && active_fs.load() < fs_list.size()) cv.notify_all();
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

/***
 * Change Shared_Queue status.
 * If false the program will shutdown gracefully
 * @param flag
 */
void SharedQueue::setFlag(bool flag) {
    std::lock_guard<std::mutex> l(m);
    SharedQueue::flag = flag;
    cv.notify_all();
}

/***
 * Get Shared Queue flag
 * @return flag
 */
bool SharedQueue::isFlag() const {
    return flag;
}
