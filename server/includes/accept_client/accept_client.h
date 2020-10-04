//
// Created by fede on 9/21/20.
//
#pragma once
#define BACKLOG_SIZE 10

/// This class act as a dispatcher. It is composed of a passive boost socket.
/// It has a list of incoming connection. It deals with them by
/// creating a thread for each one.
class AcceptClient{
public:
    AcceptClient();
    void ReadCredential();

};