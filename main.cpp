#include "parts/encryption.h"
#include "parts/utils.h"

#include <memory>

using namespace std;

int main(int argc, char* argv[]) {

    int opt;
    char state = 's';
    bool verbose = false;
    const char* ip = "";
    
    unique_ptr<Comms> c;
    CryptoManager m;

    while((opt = getopt(argc, argv, "schva:")) != -1) {
        if(opt == 's' || opt == 'c') state = opt;

        else if(opt == 'h') {
            std::cout << "'s' : Boot as a server. 'c' : Boot as a client. 'v' : Verbose." << std::endl;;
            return 0;
        } 

        else if(opt == 'v') verbose = true;

        else if(opt == 'a') ip = optarg;

        else return 1;
    }

    if(ip == "") ip = "192.168.20.30"; // my default rpi ip, change if you're using this yourself.

    c = make_unique<Comms>(state, m, ip, verbose);
    
    if(state == 's') {

        // boot & act as server

        if(c->pid == 0) {
            dup2(c->ptc[0], STDIN_FILENO);
            dup2(c->ctp[1], STDOUT_FILENO);

            close(c->ptc[1]);
            close(c->ctp[0]);

            // this branch gets replaced w the shell interface
            execl("/bin/sh", "sh", NULL);
        }

        close(c->ptc[0]);
        close(c->ctp[1]);

        cout << "Server online at ip : " << c->getOwnIP() << "\n";

        string message = "";
        string response = "";
        bool running = true;
        
        while(message != "quit" && running) {
            message = c->getEncryptedMessage();
            cout << "message from client : " << message << "\n";

            cout << "response: ";
            if(message == "quit") {
                running = false;
                response = "Quitting now...\n __FINISHED__";
                c->sendEncryptedMessage(response);
                
            } else if (message == "mr penis") {
                response = "mr balls\n __FINISHED__";
                c->sendEncryptedMessage(response);
            } else {
                message += "; echo __FINISHED__ \n";
                write(c->ptc[1], message.c_str(), message.length());

                char buffer[896];
                response = "";
                ssize_t bytesRead;

                while((bytesRead = read(c->ctp[0], buffer, sizeof(buffer)-1)) > 4) {
                    buffer[bytesRead] = '\0';
                    response = buffer;
                    
                    // cout << "SENDING :: " << response;
                    c->sendEncryptedMessage(response);
                    if(response.find("__FINISHED__") != std::string::npos) {
                        response.erase(response.find("__FINISHED__"));
                        cout << response;
                        break;
                    } else cout << response;
                }
                
            }
            // cout << "response : " << response << "\n";
            // c->sendEncryptedMessage(response);
        }


    } else {

        // boot & act as client

        cout << "Client online, communicating with " << c->ip << "\n";

        string message = "";
        string response;
        bool running = true;
        ssize_t last;

        while(message != "quit" && running) {
            cout << ">>>  ";

            if (!std::getline(std::cin, message)) break;
            last = message.find_last_not_of(" \t\n\r");

            if(last != std::string::npos) message = message.substr(0, last+1);
            else continue;

            c->sendEncryptedMessage(message);
            response = c->getEncryptedMessage();
            bool lastMessage = false;
            //cout << "initial : " << response << endl;

            if(response != "") {
                cout << "Response from server: ";
                do{
                    lastMessage = response.find("__FINISHED__") != std::string::npos;
                    if(lastMessage) response.erase(response.find("__FINISHED__"));
                    
                    // response += '\n';
                    cout << response;
                    
                    // if(lastMessage) cout << "last message!!!!!";
                    if(!lastMessage) response = c->getEncryptedMessage();
                } while(!lastMessage);
                // cout << "Response from server: " << response;
            }

            if(message == "quit") running = false;
        }

    }

    return 0;

    

}
