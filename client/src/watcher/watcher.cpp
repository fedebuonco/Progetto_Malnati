#include "watcher.h"
#include <files.h>
#include <database.h>
#include <utility>


Watcher::Watcher(std::filesystem::path  db_file, std::filesystem::path folder_watched) :
    db_file_(std::move(db_file)),
    folder_watched_(std::move(folder_watched))
{

}

/// This method will assign the callback called in the Watcher::listenerFunction() to the provided function.
/// \param updateCallback The function that will be called as the callback.
void Watcher::SetUpdateCallback(const std::function<void()> &updateCallback) {
    update_callback = updateCallback;
}

/// This is the function that will be called when a change is recorded.
/// Prints the changes and then call the provided callback ( provided from Watcher::SetUpdateCallback() )
/// \param events The series of events (file deleted, added, etc.) that triggered the function.
void Watcher::listenerFunction(const std::vector<pfw::EventPtr>& events)
{
    //We open a db connection for all the events
    DatabaseConnection db(db_file_,folder_watched_);

    //For each event
    for (const auto &event : events) {
        //std::cout << event->relativePath << " with the type: " << typeBits << std::endl;

        if (event->relativePath.string() == ".hash.db" && events.size() == 1)
            return;
    }

    update_callback();

}

/// Start the watcher.
/// \param path
void Watcher::Start(const std::filesystem::path& path){
    _watcher = std::make_unique<pfw::FileSystemWatcher>(
            path, std::chrono::milliseconds(1000),
            std::bind(&Watcher::listenerFunction, this,
                      std::placeholders::_1));
}
