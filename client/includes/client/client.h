//
// Created by fede on 10/12/20.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H


#include <config.h>
#include <filesystem>
#include <tree_t.h>
#include <patch.h>
#include <watcher.h>

class Client {
private:
    RawEndpoint server_re_;
    std::filesystem::path folder_watched_;
    Watcher watcher_;

    bool Auth();
    void StartWatching();
    void Syncro();
    TreeT RequestTree();
    std::string GenerateTree(const std::filesystem::path& path);
    Patch GeneratePatch(std::filesystem::path mon_folder,
                        const std::string& client_t,
                        const std::string& server_t);
    void ProcessNew(Patch& patch);
    void ProcessRemoved(Patch& patch);
    void SendPatch(Patch &update);
    void ProcessCommon(Patch &patch, TreeT server_treet);


public:
    Client(RawEndpoint re, std::filesystem::path folder_watched);




};


#endif //CLIENT_CLIENT_H
