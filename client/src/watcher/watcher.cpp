
#include "watcher.h"

Watcher::Watcher(Client &client) : client_to_update(client) {
}

void Watcher::SetUpdateCallback(const std::function<int(Client)> &updateCallback) {
    update_callback = updateCallback;
}

void Watcher::listenerFunction(std::vector<pfw::EventPtr> events)
{
    for (const auto &event : events) {
        std::bitset<16> typeBits(event->type);
        std::cout << event->relativePath << " with the type: " << typeBits
                  << std::endl;
    }

    update_callback(client_to_update);

}

void Watcher::Start(std::filesystem::path path){
    _watcher = std::make_unique<pfw::FileSystemWatcher>(
            path, std::chrono::milliseconds(1),
            std::bind(&Watcher::listenerFunction, this,
                      std::placeholders::_1));
}
