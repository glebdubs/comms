#pragma once

#include "encryption.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

class Comms {
private:
    int clientSocket;
    int serverSocket;
    sockaddr_in serverAddress;
    std::string ip;

    CryptoManager* manager;

    CryptoManager::EVP_PKEY_ptr publicKey;
    CryptoManager::EVP_PKEY_ptr privateKey;

    CryptoManager::EVP_PKEY_ptr foreignPublicKey;

public:
    Comms(char state, 
        CryptoManager& m,
        const char* ip = "192.168.20.30",  // default ip value is my own raspi
        const char* publicFN = "public.pem",
        const char* privateFN = "private.pem");
    bool ping();
    std::string getOwnIP();
    std::string getMessage();
    void sendMessage(std::string m);
    void sendBytes(const std::vector<unsigned char>& bytes);
    std::string getEncryptedMessage();
    void sendEncryptedMessage(std::string plaintext);
    bool doesThisFileExist(const std::string& name);

};
