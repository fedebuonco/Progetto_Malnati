#pragma once


#include <config.h>
#include <filesystem>
#include <tree_t.h>
#include <patch.h>
#include <watcher.h>
#include <sync_tcp_socket.h>
#include <control_message.h>

/// The main class of the client. It represent an object that will connect to the server specified
/// in the constructor and will ask to a watcher to watch a specified folder.
class Client {

    RawEndpoint server_re_;
    std::filesystem::path folder_watched_;
    std::filesystem::path db_file_;
    Watcher watcher_;

    bool Auth();
    void StartWatching();
    void Syncro();
    int  RecoverSending();
    void SendRemoval(Patch &update);
    TreeT RequestTree();

    bool SyncWriteCM(SyncTCPSocket& stcp, ControlMessage& cm);
    ControlMessage SyncReadCM(SyncTCPSocket& stcp);
    void InitHash();

public:
    Client(RawEndpoint re, const std::filesystem::path& folder_watched);


};


