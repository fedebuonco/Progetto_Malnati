#include "sender.h"


void Sender::insert(const FileSipper& s){
    shared_queue.get()->add(s);
}

void Sender::Sender_Action(){

    //TODO: is a good approach having here the working threadpool?

    while(1){
         if(count > N){
            std::unique_lock<std::mutex> l(shared_queue->m);
            cv.wait(l, [this](){ return count > N; } );
         }

        if(shared_queue->size() > 0){

            threads->emplace_back( [this] () {
                    //shared_queue->extract().Start(); //Start File Sipper

            });




        }



    }

}
