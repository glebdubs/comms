#pragma once

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

class Comms {
private:
    int clientSocket;
    sockaddr_in serverAddress;
    std::string ip;

public:
    Comms(const char* ip = "192.168.20.30"); // default value is my own raspi
    void ping();
    std::string getMessage();
    void sendMessage(std::string m);
    bool doesThisFileExist(const std::string& name);

};