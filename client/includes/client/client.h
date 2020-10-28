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
    Patch GeneratePatch(std::string basicString, std::string basicString1);
    void SendPatch(Patch update);

    void AddTime(Patch patch);

    void ProcessNew(Patch patch);

    void ProcessRemoved(Patch patch);

private:
    void Run(unsigned short port_num);

};


#endif //CLIENT_CLIENT_H
