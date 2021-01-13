#include "sender.h"

const unsigned int DEFAULT_THREAD_POOL_SIZE = 1;

Sender *Sender::m_Sender = nullptr;

///
/// \param s
////
/////insert(std::packaged_task< bool ( FileSipper ) > task, filesipper, ........);
/*{
 * //creofilesipper
 * submit(task, filesippee) ???????
 * }
 * */
void Sender::insert(std::shared_ptr<FileSipper> s){
    shared_queue.get()->add(s);
}

void Sender::Sender_Action(){

    //TODO: is a good approach having here the working threadpool?

    // Launch the pool with four threads.
    unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
    if (thread_pool_size == 0) {
        thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
    }


    boost::asio::thread_pool pool(thread_pool_size);


    while(flag){

        //choose one: use shared_queue method to find if there is a ready filesipper to be sent
        std::shared_ptr<FileSipper> choosen_fs;// = SharedQueue::get_ready_FileSipper();

        //cv.wait(l, [this](){ return shared_queue->size()>0 && ( shared_queue->size() - shared_queue->getActiveFileSipper() ) > 0 ; } );
        //if(shared_queue->size()>0 && ( shared_queue->size() - shared_queue->getActiveFileSipper() ) > 0)
        boost::asio::post(pool, [choosen_fs](){

            ////
            // Fetch the associated future<> from packaged_task<>
            ///std::future<std::bool> result = pop.get_future();
            // Pass the packaged_task to thread to run asynchronously
            ////std::thread th(std::move(task), "Arg");

            //TODO: why not std::packaged_task ( we're using std::queue<std::shared_ptr<FileSipper>> )

            //choosen_fs->FileSipper::Start();
            // e aggiorno db
        });

    }

    // Wait for all tasks in the pool to complete.
    pool.join();

}

Sender *Sender::get_Instance() {
        if(!m_Sender){
            m_Sender = new Sender();
        }
        return m_Sender;
}

void Sender::setSharedQueue(const std::shared_ptr<SharedQueue> &sharedQueue) {
    shared_queue = sharedQueue;
}
