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

/// The watcher is a wrapper for panoptes watcher library, will take a folder and will check for changes in the file structure.
class Watcher{

    public:

    void SetUpdateCallback(const std::function<void()> &updateCallback);
    void listenerFunction(std::vector<pfw::EventPtr> events);
    void Start(std::filesystem::path path);

    private:
        std::unique_ptr<pfw::FileSystemWatcher> _watcher;
        std::function<void()> update_callback;
};




#endif //CLIENT_WATCHER_H
