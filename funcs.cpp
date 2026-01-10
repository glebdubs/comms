#include "funcs.h"

using namespace std;

Comms::Comms(const char* ip) {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket < 0) {
        std::cerr << "Error reading socket. \n";
        return;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    if(inet_pton(AF_INET, ip, &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid Address / Address not supported. \n";
        return;
    }

    std::cout << "Connecting... \n";

    if(connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed. Is the server running, dumbass? \n";
        return;
    }
    std::cout << "Connection successfully established.";
}

void Comms::ping() {

}

std::string Comms::getMessage() {

}

void Comms::sendMessage(std::string m) { // ADD ENCRYPTION PROCESS TO THIS
    const char* msg = m.c_str();
    send(clientSocket, msg, strlen(msg), 0);
}