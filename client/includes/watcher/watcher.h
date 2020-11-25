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

class Watcher{
    public:

        void listenerFunction(std::vector<pfw::EventPtr> events)
        {
            for (const auto &event : events) {
                std::bitset<16> typeBits(event->type);
                std::cout << event->relativePath << " with the type: " << typeBits
                          << std::endl;
            }
        }
        void Start(std::filesystem::path path){
            _watcher = std::make_unique<pfw::FileSystemWatcher>(
                    path, std::chrono::milliseconds(1),
                    std::bind(&Watcher::listenerFunction, this,
                              std::placeholders::_1));
        }

    private:
        std::unique_ptr<pfw::FileSystemWatcher> _watcher;
    };




#endif //CLIENT_WATCHER_H
