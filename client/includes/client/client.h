#pragma once


#include <config.h>
#include <filesystem>
#include <tree_t.h>
#include <patch.h>
#include <watcher.h>
#include <sync_tcp_socket.h>
#include <control_message.h>
#include <database.h>

/// The main class of the client. It represent an object that will connect to the server specified
/// in the constructor and will ask to a watcher to watch a specified folder.
class Client {

    /// Raw endpoint containing the server ip and port.
    RawEndpoint server_re_;
    /// Folder that we are monitoring.
    std::filesystem::path folder_watched_;
    /// File path of the DB file.
    std::filesystem::path db_file_;
    /// Watcher that will take care of monitoring the folder. Wrapper of the Panoptes watcher.
    Watcher watcher_;

    bool Auth();
    void StartWatching();
    void Syncro();
    int  RecoverSending();
    void SendRemoval(Patch &update);
    TreeT RequestTree();

    void SyncWriteCM(SyncTCPSocket& stcp, ControlMessage& cm);
    ControlMessage SyncReadCM(SyncTCPSocket& stcp);
    void InitHash();
    std::string HashFile(const std::filesystem::path& path);

public:
    Client(RawEndpoint re, const std::filesystem::path& folder_watched);


};


