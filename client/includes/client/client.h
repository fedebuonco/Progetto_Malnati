#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H


#include <config.h>
#include <filesystem>
#include <tree_t.h>
#include <patch.h>
#include <watcher.h>
#include <sync_tcp_socket.h>
#include <control_message.h>

/// The main class of the client.
/// It represent an object that will connect to the server
/// specified in the constructor and will
/// ask to a watcher to watch a specified folder.
class Client {

private:
    RawEndpoint server_re_;
    std::filesystem::path folder_watched_;
    std::filesystem::path db_file_;
    Watcher watcher_;

    bool Auth();
    void StartWatching();
    void Syncro();
    void SendPatch(Patch &update);
    TreeT RequestTree();
    bool SyncWriteCM(SyncTCPSocket& stcp, ControlMessage& cm);
    ControlMessage SyncReadCM(SyncTCPSocket& stcp);
public:
    Client(RawEndpoint re, std::filesystem::path folder_watched);

    void InitHash();


};

#endif //CLIENT_CLIENT_H
