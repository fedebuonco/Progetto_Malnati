#include <shared_queue.h>

SharedQueue *SharedQueue::m_SharedQueue = nullptr;

SharedQueue *SharedQueue::get_Instance() {
    if(!m_SharedQueue){
        m_SharedQueue = new SharedQueue();
    }
    return m_SharedQueue;
}

/***
 * Get the size of the SharedQueue, thread safe
 * @return int size : number of element inside the Queue
 */
/*int SharedQueue::size(){
    std::lock_guard<std::mutex> l(m);
    return fs_list.size();
}*/

/**
 * Choose a FileSipper from the queue that is ready to send a file.
 * If queue is empty or all FileSipper are already started, Sender will be waiting.
 * @return std::shared_ptr<FileSipper>
 */
std::shared_ptr<FileSipper> SharedQueue::get_ready_FileSipper(){
    std::unique_lock<std::mutex> l(m);

    //While is necessary to prevent spurious wakeup.
    //Thread waits on cv when queue is empty OR all files in fileSippers are in sending.
    //Check also the Shared Queue status flag; if true we need to wakeup to close the program
                                                                            //TODO: We are inside shared queue, so that flag directly? @marco
    while( ( fs_list.empty() || (fs_list.size() == active_fs.load())) && SharedQueue::get_Instance()->isFlag()  ) {
        cv.wait(l, [this]() { return !( (fs_list.empty() || (fs_list.size() == active_fs.load())) && SharedQueue::get_Instance()->isFlag() ); });
    }

    if( !SharedQueue::get_Instance()->isFlag() ){
        return nullptr;     //TODO: Va bene ritornare nullptr? @marco
    }

    //If we are here, it means that we have a fileSipper ready and a thread can handle it

    //Search for next ready FileSipper.
    std::list<std::shared_ptr<FileSipper>>::iterator it;
    for (it = fs_list.begin(); it != fs_list.end(); ++it){

        //We search only the fileSipper with status 'false'; they are not handled yet
        if(!it->get()->status.load()) {

            //We found one that will be handle; so we change his status and increment the active counter
            it->get()->status.store(true);
            active_fs.fetch_add(1);
            return *it;
        }
    }

    //If we are here, it means that there was a problem.
    return nullptr;//TODO: Va bene ritornare nullptr? @marco
}

/***
 * Remove the selected fileSipper from the shared queue.
 * @param fileSipper to be removed
 */
void SharedQueue::remove_element(const std::shared_ptr<FileSipper>& file_sipper){
    std::lock_guard<std::mutex> l(m);

    //We remove the file_sipper from the list because it finished correctly his work and decrement the active_fs counter.
    fs_list.remove(file_sipper);
    SharedQueue::get_Instance()->active_fs.fetch_sub(1);         //TODO: We are inside shared queue, @marco

    //After we remove the element, we check if the list is NOT empty and if we are some to handle (i.e. the size is less that active)
    if(!fs_list.empty() && active_fs.load() < fs_list.size()) cv.notify_all();
}

/***
 * Add a new ptrFileSipper in the SharedQueue
 * @param file_sipper to be added
 */
void SharedQueue::insert(const std::shared_ptr<FileSipper>& file_sipper){

    //Take the lock and insider the file_sipper inside the list
    std::lock_guard<std::mutex> l(m);
    fs_list.push_front(file_sipper);

    //We do notify all to wakeup all thread waiting on a cv because there is a file_sipper to handle.
    cv.notify_all();
}

/***
 * Change Shared_Queue status. If false the program will shutdown gracefully
 * @param flag_value
 */
void SharedQueue::setFlag(bool flag_value) {
    std::lock_guard<std::mutex> l(m);
    flag = flag_value;

    //We do notify all to wakeup all the thead waiting on a cv. They will wakeup to terminate the program.
    cv.notify_all();
}

/***
 * Get Shared Queue flag
 * @return flag
 */
bool SharedQueue::isFlag() {
    std::lock_guard<std::mutex> l(m);           //TODO: Aggiunto un mutex per leggere bene flag. Corretto? @marco
    return flag;
}
