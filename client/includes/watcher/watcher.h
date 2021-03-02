#pragma once

#include <filesystem>
#include <vector>
#include <pfw/FileSystemWatcher.h>
#include <pfw/NativeInterface.h>
#include <bitset>

/// The watcher is a wrapper for panoptes watcher library, will take a folder and will check for changes in the file structure.
class Watcher{
    /// DB file
    std::filesystem::path db_file_;
    /// Folder monitored
    std::filesystem::path folder_watched_;
    /// Panoptes Watcher
    std::unique_ptr<pfw::FileSystemWatcher> _watcher;
    /// Callback called when change is detected.
    std::function<void()> update_callback;
public:

    void SetUpdateCallback(const std::function<void()> &updateCallback);
    void listenerFunction(const std::vector<pfw::EventPtr>& events);
    void Start(const std::filesystem::path& path);
    Watcher(std::filesystem::path  db_file, std::filesystem::path folder_watched);

};
