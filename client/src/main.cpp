/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/

/* 00 system includes */

#include <iostream>

/* 01 project includes */

#include "connectToServer.h"
#include "../includes/config/config.h"

/* 02 externs */
/* 03 defines */
/* 04 typedefs */
/* 05 globals (but don't)*/
/* 06 ancillary function prototypes if any */

int main(int argc, char *argv[])
{

    if(! Config::get_Instance()->isConfig() ){

        Config::get_Instance()->startConfig();
    }

        
    connectToServer connector;

/* 07 variable declarations */
/* 08 check argv[0] to see how the program was invoked */
/* 09 process the command line options from the user */
/* 10 do the needful */
}
