#include <sender.h>

const unsigned int DEFAULT_THREAD_POOL_SIZE = 1;


 /// Thread Pool creation, fileSipper selection to be sent.
 /// After the choice of the fileSipper to be sent, it is assigned to a thread of the pool. It is removed from the
 /// shared queue.
void Sender::Sender_Action() const{

    //Calculate the size of the pool based on the hardware
    unsigned int thread_pool_size = std::thread::hardware_concurrency();// * 2;
    if (thread_pool_size == 0) thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

    // Launch the pool with size threads.
    boost::asio::thread_pool pool(thread_pool_size);

    while(flag){

        // Choose a ready fileSipper to be sent. If no one is available it waits on a condition variable.
        std::shared_ptr<FileSipper> chosen_fs = SharedQueue::get_Instance()->get_ready_FileSipper();

        // If we have a nullptr, we are shutting down the sender or we don't have any fileSipper to work on.
        // Either cases we woke up the thread waiting on cv.
        // If we are terminating the program, the next while cycle will not be done.
        if(chosen_fs!=nullptr){
            //We take a thread from the pool and assign it the previously chosen fileSipper
            boost::asio::post(pool,[chosen_fs](){
                try {
                    chosen_fs->Send([&chosen_fs](){SharedQueue::get_Instance()->remove_element(chosen_fs);});
                    //SharedQueue::get_Instance()->remove_element(chosen_fs);
                }
                catch (std::exception &e){
                    //Here we have the exception that have been throw while creating and sending files
                    SharedQueue::get_Instance()->remove_element(chosen_fs);
                    if(DEBUG) std::cerr << "Error unable to send a file; It is possible that the file has been removed or a generic error occurred. \n" << e.what() << std::endl;
                    return;
                }
            });
        }
    }

    //If we are here it means that we request to shutdown the program

    //Wait for all tasks in the pool to complete, before exit
    pool.join();

}

 /// Change Sender flag status. If false the program will shutdown gracefully
 /// \param flag_value: new flag value
void Sender::setFlag(bool flag_value) {
   flag = flag_value;
}




