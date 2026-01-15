// C++ program to show the example of server application in
// socket programming
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

std::vector<std::string> getWords(char* orig)  {
    // TO IMPLEMENT
}

int main()
{
    // creating socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        perror("socket creation failed");
        return -1;
    }

    // <serverspecific>
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // </serverspecific>

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // <serverspecific>
    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind failed, try a different port or check if its in use.");
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

    std::string orig = "";
    while(orig != "quit") {
        char buffer[1024] = {0};
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if(bytesRead > 0) {
            std::cout << "message from client : " << buffer << std::endl;
            
            
        } else {
            orig = "quit";
            std::cout << "shutting down server. \n";
        }

    }

    const char* returnMessage = "yup we got you twin";
    send(clientSocket, returnMessage, strlen(returnMessage), 0);

    close(clientSocket);
    close(serverSocket);

    return 0;

}