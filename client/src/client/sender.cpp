#include "sender.h"

const unsigned int DEFAULT_THREAD_POOL_SIZE = 1;

void Sender::Sender_Action(){
    // Launch the pool with size threads.
    unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
    if (thread_pool_size == 0) {
        thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
    }


    boost::asio::thread_pool pool(thread_pool_size);

    while(flag){

        //choose one: use shared_queue method to find if there is a ready filesipper to be sent
        std::shared_ptr<FileSipper> choosen_fs = SharedQueue::get_Instance()->get_ready_FileSipper();

        boost::asio::post(pool,[choosen_fs](){
                            choosen_fs->Send();
                            SharedQueue::get_Instance()->remove_element(choosen_fs);
        });

     }

    // Wait for all tasks in the pool to complete.
    pool.join();

}

/***
 * Change Sender flag status.
 * If false the program will shutdown gracefully
 * @param flag
 */
void Sender::setFlag(bool flag) {
    Sender::flag = flag;
}




