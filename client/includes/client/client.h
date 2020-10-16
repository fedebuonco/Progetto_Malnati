//
// Created by fede on 10/12/20.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H


#include <config.h>

class Client {
private:
    RawEndpoint server_re_;


public:
    Client(RawEndpoint re);
    void Stop();

    bool Auth();

    void RequestTree();

private:
    void Run(unsigned short port_num);
};


#endif //CLIENT_CLIENT_H
