//
// Created by fede on 9/21/20.
//

#ifndef CLIENT_WATCHER_H
#define CLIENT_WATCHER_H


#include <filesystem>
#include <vector>
#include <pfw/FileSystemWatcher.h>
#include <pfw/NativeInterface.h>
#include <bitset>
#include "client.h"
class Watcher{

    public:

    Watcher(Client &client);
    void SetUpdateCallback(const std::function<int(Client)> &updateCallback);
    void listenerFunction(std::vector<pfw::EventPtr> events);
    void Start(std::filesystem::path path);

    private:
        std::unique_ptr<pfw::FileSystemWatcher> _watcher;
        std::function<int(Client)> update_callback;
        Client& client_to_update;
};




#endif //CLIENT_WATCHER_H
