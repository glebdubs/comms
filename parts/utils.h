#pragma once

#include "encryption.h"

#include <cstring>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <chrono>
#include <thread>

class Comms {
private:
    bool verbose;
    int clientSocket;
    int serverSocket;
    sockaddr_in serverAddress;
    
    CryptoManager* manager;
    
    CryptoManager::EVP_PKEY_ptr publicKey;
    CryptoManager::EVP_PKEY_ptr privateKey;
    CryptoManager::EVP_PKEY_ptr foreignPublicKey;
    
    
public:

    int ptc[2];
    int ctp[2];
    pid_t pid;
    
    std::string ip;

    Comms(char state, 
        CryptoManager& m,
        const char* ip = "192.168.20.30",  // default ip value is my own raspi
        bool verbose = false,
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
