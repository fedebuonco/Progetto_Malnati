#include "sender.h"


void Sender::insert(const FileSipper& s){
    shared_queue.get()->add(s);
}
