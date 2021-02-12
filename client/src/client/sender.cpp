#include <sender.h>

const unsigned int DEFAULT_THREAD_POOL_SIZE = 1;

void Sender::Sender_Action() const{

    //Calculate the size of the pool based on the hardware
    unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
    if (thread_pool_size == 0) thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

    // Launch the pool with size threads.
    boost::asio::thread_pool pool(thread_pool_size);

    while(flag){

        //Choose a ready fileSipper to be sent. If no one is available it waits on a condition variable.
        std::shared_ptr<FileSipper> chosen_fs = SharedQueue::get_Instance()->get_ready_FileSipper();

        if(chosen_fs==nullptr){
            //If we are here, it means that we woke up the thread waiting on cv in order to terminate the program.
            //Continue because in the next iteration we exit from the while (condition will be false) and join the threads.
            std::cout<<"NULLPTR"<<std::endl;
            continue;
        }

        //We take a thread from the pool and assign it the previously chosen fileSipper
        boost::asio::post(pool,[chosen_fs](){

                try {
                    chosen_fs->Send([chosen_fs](){SharedQueue::get_Instance()->remove_element(chosen_fs);});
                    //SharedQueue::get_Instance()->remove_element(chosen_fs);
                }
                catch (std::exception &e){
                    //Here we have the exception that have been throw while creating and sending files
                    //TODO: I have a lot of prints with cerr; for me we can remove the cerr. @marco
                    SharedQueue::get_Instance()->remove_element(chosen_fs);
                    std::cerr << "Error Unable to send a file; maybe was deleted thread " << e.what() << std::endl;
                    return;
                }
        });

    }

    //If we are here it means that we request to shutdown the program

    //Wait for all tasks in the pool to complete, before exit
    pool.join();

}

/***
 * Change Sender flag status. If false the program will shutdown gracefully
 * @param flag_value
 */
void Sender::setFlag(bool flag_value) {
   flag = flag_value;
}




