#include "sender.h"

const unsigned int DEFAULT_THREAD_POOL_SIZE = 1;


///
/// \param s
////
/////insert(std::packaged_task< bool ( FileSipper ) > task, filesipper, ........);
/*{
 * //creofilesipper
 * submit(task, filesippee) ???????
 * }
 * */

void Sender::Sender_Action(){

    //TODO: is a good approach having here the working threadpool?

    // Launch the pool with four threads.
    unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
    if (thread_pool_size == 0) {
        thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
    }


    boost::asio::thread_pool pool(thread_pool_size);


    while(flag){


        std::cout << std::this_thread::get_id << " "<<std::endl;
        //choose one: use shared_queue method to find if there is a ready filesipper to be sent
        std::shared_ptr<FileSipper> choosen_fs = SharedQueue::get_Instance()->get_ready_FileSipper();

        boost::asio::post(pool, [choosen_fs](){

            std::cout << std::this_thread::get_id << "  pool "<<std::endl;

            choosen_fs->Send();
            std::cout << " FACCIO PARTIRE FS SCELTO  " << choosen_fs->file_string_ << std::endl;

            // shared

            SharedQueue::get_Instance()->remove_element(choosen_fs);

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

    std::cout<<" join pool"<<std::endl;
    // Wait for all tasks in the pool to complete.
    pool.join();

}

/// MUTEX ?????
void Sender::setFlag(bool flag) {
    Sender::flag = flag;
}

bool Sender::isFlag() const {
    return flag;
}



