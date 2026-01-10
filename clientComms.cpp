// C++ program to illustrate the client application in the
// socket programming
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket < 0) {
        std::cerr << "Error creating socket " << std::endl;
        return -1;
    }

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    // serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(inet_pton(AF_INET, "192.168.20.30", &serverAddress.sin_addr) <= 0) {
        std::cerr << "invalid address / address not supported \n";
        return -1;
    }

    std::cout << "connecting...\n";

    if(connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "connection failed. is the server running, dumbass? \n";
        return -1;
    }

    // sending connection request
    connect(clientSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));

    // sending data
    const char* message = "hi server";

    std::cout << "sending...\n";
    send(clientSocket, message, strlen(message), 0);
    std::cout << "sent.\n";

    // closing socket
    close(clientSocket);

    return 0;
}