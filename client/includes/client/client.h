//
// Created by fede on 10/12/20.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H


#include <config.h>
#include <filesystem>
#include <tree_t.h>
#include <patch.h>

class Client {
private:
    RawEndpoint server_re_;


public:
    Client(RawEndpoint re);
    void Stop();
    bool Auth();
    TreeT RequestTree();
    std::string GenerateTree(const std::filesystem::path& path);
    Patch GeneratePatch(const std::string& client_t,const std::string& server_t);
    void ProcessNew(Patch& patch);
    void ProcessRemoved(Patch& patch);
    void SendPatch(Patch &update);
private:
    void Run(unsigned short port_num);


    void ProcessCommon(Patch &patch, TreeT server_treet);
};


#endif //CLIENT_CLIENT_H
