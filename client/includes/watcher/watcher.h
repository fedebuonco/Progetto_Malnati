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
    void listenerFunction(const std::vector<pfw::EventPtr>& events);
    void Start(const std::filesystem::path& path);
    Watcher(std::filesystem::path  db_file, std::filesystem::path folder_watched);

    private:

    std::filesystem::path db_file_;
    std::filesystem::path folder_watched_;

    std::unique_ptr<pfw::FileSystemWatcher> _watcher;
    std::function<void()> update_callback;
};




#endif //CLIENT_WATCHER_H
