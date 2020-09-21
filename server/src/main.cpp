#include <iostream>
#include "../config/ConfigClass.h"
#include "utilities/ut.h"

int main() {

    //Check if the app is configured correctly
    if ( !ConfigClass::isConfig() ){
        if (DEBUG) std::cout << "No configuration found; user is not logged in" << std::endl;


        ConfigClass::startConfig();
    }


    return 0;
}
