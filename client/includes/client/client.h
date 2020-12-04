#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H


#include <config.h>
#include <filesystem>
#include <tree_t.h>
#include <patch.h>
#include <watcher.h>

/// The main class of the client.
/// It represent an object that will connect to the server
/// specified in the constructor and will
/// ask to a watcher to whatch a specified folder.
class Client {

private:
    RawEndpoint server_re_;
    std::filesystem::path folder_watched_;
    Watcher watcher_;

    bool Auth();
    void StartWatching();
    void Syncro();
    void SendPatch(Patch &update);
    TreeT RequestTree();
public:
    Client(RawEndpoint re, std::filesystem::path folder_watched);
};

#endif //CLIENT_CLIENT_H
