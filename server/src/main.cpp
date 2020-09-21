#include <iostream>
#include "../config/config.h"
#include "utilities/ut.h"

int main() {

    //Check if the app is configured correctly
    if ( !config::get_Instance()->isConfig() ){
        if (DEBUG) std::cout << "No configuration found; user is not logged in" << std::endl;

        config::get_Instance()->startConfig();
    }


    return 0;
}

