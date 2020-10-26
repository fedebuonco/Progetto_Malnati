//
// Created by fede on 10/12/20.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H


#include <config.h>
#include <filesystem>
#include <tree_h.h>
#include <patch.h>

class Client {
private:
    RawEndpoint server_re_;


public:
    Client(RawEndpoint re);
    void Stop();
    bool Auth();
    TreeH RequestTree();
    std::string GenerateTree(const std::filesystem::path& path);
    Patch GeneratePatch(std::string basicString, std::string basicString1);
private:
    void Run(unsigned short port_num);
};


#endif //CLIENT_CLIENT_H
