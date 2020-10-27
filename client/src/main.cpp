/* main.cpp - Backup-Client, the client used in our app in order to start the synchronization*/
/* 00 system includes */
#include <iostream>
/* 01 project includes */
#include "config.h"
#include "../includes/client/client.h"
#include <boost/property_tree/ptree.hpp>
#include <tree_h.h>
#include <patch.h>

int main(int argc, char *argv[])
{
    RawEndpoint raw_endpoint = Config::get_Instance()->ReadRawEndpoint();

    // Creating the Client and Auth
    Client client{raw_endpoint};
    client.Auth();

    // From here on, we are authenticated.
    // Here we and start the monitor where
    // we loop detecting a change in the dir.

    std::string client_tree;

    //Generate Client tree string
    client_tree = client.GenerateTree(std::filesystem::path("Prova"));

    // Then we ask for the Server's TreeH ( Tree string and Hashes )
    TreeH server_th = client.RequestTree();

    //And we can compute the Diff and store it in a Patch
    Patch update = client.GeneratePatch(client_tree, server_th.tree_);

    //debug
    update.PrettyPrint();

    // client.ProcessRemoved(update);
    // client.ProcessCommon(update,map hash);
     client.ProcessNew(update);


    // Here the main has everything it needs in order to asyncronously send the patch
    //client.SendPatch(patch);
    
}
