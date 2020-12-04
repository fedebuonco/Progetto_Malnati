
#include "watcher.h"
/// This method will assign the callback called in the Watcher::listenerFunction() to the provided function.
/// \param updateCallback The function that will be called as the callback.
void Watcher::SetUpdateCallback(const std::function<void()> &updateCallback) {
    update_callback = updateCallback;
}
/// This is the function that will be called when a change is recorded.
/// Prints the changes and then call the provided callback ( provided from Watcher::SetUpdateCallback() )
/// \param events The series of events (file deleted, added, etc.) that triggered the function.
void Watcher::listenerFunction(std::vector<pfw::EventPtr> events)
{
    for (const auto &event : events) {
        std::bitset<16> typeBits(event->type);
        std::cout << event->relativePath << " with the type: " << typeBits
                  << std::endl;
    }

    update_callback();

}
/// Start the watcher.
/// \param path
void Watcher::Start(std::filesystem::path path){
    _watcher = std::make_unique<pfw::FileSystemWatcher>(
            path, std::chrono::milliseconds(1),
            std::bind(&Watcher::listenerFunction, this,
                      std::placeholders::_1));
}
