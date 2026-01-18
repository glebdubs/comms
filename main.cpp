#include "encryption.h"
#include "utils.h"

#include <memory>

using namespace std;

int main(int argc, char* argv[]) {

    int opt;
    char state = 's';
    bool verbose = false;
    const char* ip = "";

    while(int opt = getopt(argc, argv, "schva:") != -1) {
        if(opt == 's' || opt == 'c') state = opt;

        else if(opt == 'h') {
            std::cout << "'s' : Boot as a server. 'c' : Boot as a client. 'v' : Verbose." << std::endl;;
            return 0;
        } 

        else if(opt == 'v') verbose = true;

        else if(opt == 'a') ip = optarg;

        else return 1;
    }

    unique_ptr<Comms> c;
    CryptoManager m;

    if(ip == "") c = make_unique<Comms>(state, m, ip);
    else         c = make_unique<Comms>(state, m);

    if(state == 's') {

        // boot & act as server

        cout << "Server online at ip : " << c->getOwnIP() << "\n";

        string message = "";
        string response = "";
        bool running = true;
        
        while(message != "quit" && running) {
            message = c->getEncryptedMessage();
            cout << "message from client : " << message << "\n";

            if(message == "quit") {
                running = false;
                response = "Quitting now...\n";

            } else if (message == "mr penis") {
                response = "mr balls\n";
            }
            cout << "response : " << response << "\n";
            c->sendEncryptedMessage(response);
        }


    } else {

        // boot & act as client

        cout << "Client online, communicating with " << ip << "\n";

        string message = "";
        string response;
        bool running = true;

        while(message != "quit" && running) {
            cout << ">>>  ";
            cin >> message;
            c->sendEncryptedMessage(message);
            response = c->getEncryptedMessage();
            cout << response;
            if(message == "quit") running = false;
        }

    }

    return 0;

    

}
