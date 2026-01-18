#include "utils.h"

Comms::Comms(char state, CryptoManager& m, const char* ip, const char* publicFN, const char* privateFN) 
    : clientSocket(-1), serverSocket(-1), manager(&m),
      publicKey(nullptr, &EVP_PKEY_free), 
      privateKey(nullptr, &EVP_PKEY_free),
      foreignPublicKey(nullptr, &EVP_PKEY_free) {

    privateKey = manager->loadPrivateKey(privateFN);
    publicKey  = manager->loadPublicKey (publicFN);

    if(state == 's') {

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if(serverSocket < 0) {
            std::cerr << "Socket creation failed. \n";
            std::exit(-1);

        }

        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8080);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

            if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
            std::cerr << "Bind failed, try a different port or check if its in use. \n";
            std::exit(-1);
        }

        if(listen(serverSocket, 5) < 0) {
            std::cerr << "Listen failed. \n";
            std::exit(-1);
        }

        std::cout << "Server is listening on port 8080...\n";

        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if(clientSocket < 0) {
            std::cerr << "Accept failed. \n";
            std::exit(-1);
        }

        // await ping to ensure connection stability.

        std::string pingMessage = getMessage();
        sendMessage("return ping");

        if(pingMessage == "ping") { // the illusion of choice
            std::cout << "Plaintext connection stable. Encrypting... \n";
        } else {
            std::cout << "Connection unstable. Attempting encryption... \n";
        }

        sendMessage(manager->plaintextPublicKey);


    } else if(state == 'c') {

        // creating socket
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if(clientSocket < 0) {
            std::cerr << "Error reading socket. \n";
            return;
        }

        // specifying address
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8080);

        // verifying address validity
        if(inet_pton(AF_INET, ip, &serverAddress.sin_addr) <= 0) {
            std::cerr << "Invalid Address / Address not supported. \n";
            return;
        }

        // std::cout << "Connecting... \n";

        if(connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
            std::cerr << "Connection failed. Is the server running, dumbass? \n";
            return;
        }

        // running a ping circuit to server and back to ensure connection consistency
        if(Comms::ping()) std::cout << "Connection successfully established. \n";
        else {
            std::cout << "Ping failed. Connection successful, although may be inconsistent. Take care. \n";
        }

    }

    
}

bool Comms::ping() {
    Comms::sendMessage("ping");
    if(Comms::getMessage() == "return ping")  {
        return true;
    }
    return false;
}

std::string Comms::getMessage() {
    char buffer[1024] = {0};

    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    return buffer;
}

void Comms::sendMessage(std::string m) { // ADD ENCRYPTION PROCESS TO THIS
    const char* msg = m.c_str();
    send(clientSocket, msg, strlen(msg), 0);
}

void Comms::sendBytes(const std::vector<unsigned char>& bytes) {
    send(clientSocket, bytes.data(), bytes.size(), 0);
}

std::string Comms::getEncryptedMessage() {
    std::vector<unsigned char> buffer(1024);
    recv(clientSocket, buffer.data(), buffer.size(), 0);

    std::string plaintext = CryptoManager::decrypt(privateKey.get(), buffer); 

    return plaintext;
}

void Comms::sendEncryptedMessage(std::string plaintext) {
    std::vector<unsigned char> bin(1024, 0);

    bin = CryptoManager::encrypt(foreignPublicKey.get(), plaintext);

    sendBytes(bin);
}

bool Comms::doesThisFileExist(const std::string& name) {
    if(FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

std::string Comms::getOwnIP() {
    char host[256];

    if(gethostname(host, sizeof(host)) == -1) {
        perror("Function gethostname failed. Could not establish own device IP.\n");
        std::exit(-1);
    }
    hostent* he = gethostbyname(host);
    if( he == nullptr ) {
        std::cerr << "Function gethostbyname failed. Could not establish own IP. \n";
        std::exit(-1);
    }

    in_addr* addy = (in_addr*)he->h_addr_list[0];
    
    return inet_ntoa(*addy);
}
