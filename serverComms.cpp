// C++ program to show the example of server application in
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
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        perror("socket creation failed");
        return -1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind faioled, try a different port or check if its in use.");
        return -1;
    }

    if(listen(serverSocket, 5) < 0) {
        perror("Listen failed");
        return -1;
    }

    std::cout << "server is listening on port 8080...\n";

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if(clientSocket < 0) {
        perror("Accept failed");
        return -1;
    }

    char buffer[1024] = {0};
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if(bytesRead > 0) {
        std::cout << "message from client : " << buffer << std::endl;
    }

    close(clientSocket);
    close(serverSocket);

    return 0;

    // // specifying the address
    // sockaddr_in serverAddress;
    // serverAddress.sin_family = AF_INET;
    // serverAddress.sin_port = htons(8080);

    // if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
    //     std::cerr << "invalid address / address not supported \n";
    //     return -1;
    // }

    // // binding socket.
    // bind(serverSocket, (struct sockaddr*)&serverAddress,
    //      sizeof(serverAddress));

    // // listening to the assigned socket
    // listen(serverSocket, 5);

    // // accepting connection request
    // int clientSocket = accept(serverSocket, nullptr, nullptr);

    // // recieving data
    // char buffer[1024] = { 0 };
    // recv(clientSocket, buffer, sizeof(buffer), 0);
    // cout << "Message from client: " << buffer
    //           << endl;

    // // closing the socket.
    // close(serverSocket);

    // return 0;
}