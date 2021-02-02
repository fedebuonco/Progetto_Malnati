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
    bool fs_find = false;

    //While is necessary to prevent spurious wakeups
    //Thread waits when queue is empty OR all files in filesippers are in sending
    while( ( fs_list.size()==0 || (fs_list.size() == active_fs.load())) && SharedQueue::get_Instance()->isFlag()  ) {
        //std::cout<< " wait:   fs_list.size =  "<<fs_list.size() << " and active_fs = "<<active_fs.load()<< std::endl;
        cv.wait(l, [this]() { return !( (fs_list.size()==0 || (fs_list.size() == active_fs.load())) && SharedQueue::get_Instance()->isFlag() ); });
        //std::cout << "After wait " << std::this_thread::get_id << "  pool "<<std::endl;
    }

    if( !SharedQueue::get_Instance()->isFlag() ){
        return nullptr;
    }


    std::shared_ptr<FileSipper> fs_selected;
    //std::cout<<" SIZE = " <<fs_list.size() << " ACTIVE = " << active_fs.load()<<std::endl;

    std::list<std::shared_ptr<FileSipper>>::iterator it;
    for (it = fs_list.begin(); it != fs_list.end(); ++it){
        //std::cout <<"Status filesipper " << it->get()->status.load()<<std::endl;
        if(it->get()->status.load() == false) {

            it->get()->status.store(true);
            //std::cout<<"E' false, lo scelgo : "<< it->get() <<std::endl;
            fs_find = true;
            active_fs.fetch_add(1);
            return *it;
        }
    }

   //Non dovrebbe mai saltare questo if grazie al controllo fs_list.size() == active_fs
   if(fs_find == true){
       //Faccio partire il fs
       //std::cout <<"Ritorno fs scelto" << fs_selected->status.load()<<std::endl;

       //I have the mutex

       active_fs.fetch_add(1);
       std::cout<<" ACTIVE = " <<active_fs.load()<<std::endl;

       return fs_selected;
   }


   return nullptr;

}

//TODO è necessaria?
void SharedQueue::remove_element(std::shared_ptr<FileSipper> fsipper){
    std::lock_guard<std::mutex> l(m);
    //std::cout<< std::endl << "Removing element "  << fsipper->file_string_ << std::endl;
    fs_list.remove(fsipper);
    //std::cout<< std::endl << "Removed element "  << fsipper->file_string_ << std::endl;
    //delete  fsipper.get();

    //std::cout << std::this_thread::get_id  <<"REMOVED size: " <<fs_list.size()<<std::endl;
    SharedQueue::get_Instance()->active_fs.fetch_sub(1);
    if(fs_list.size() && active_fs.load()<fs_list.size()) cv.notify_all();
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



void SharedQueue::setFlag(bool flag) {
    std::lock_guard<std::mutex> l(m);
    SharedQueue::flag = flag;
    cv.notify_all();
}

bool SharedQueue::isFlag() const {
    return flag;
}
