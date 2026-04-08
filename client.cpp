#include "common.hpp"


bool exitProg = false;
std::vector<std::string> files = {};

int ClientEntry(std::string IP, int port) {
    std::cout << IP;
    networkbase clientside;
    clientside.configureNetwork();
    clientside.setupForeignAddr(IP, port);
    SOCKET CONsock = clientside.networkinitTCP(false, false);
    if (CONsock == INVALID_SOCKET) {
        std::cout << "INITALISATION ERROR\n";
        return 1;
    }

    std::cout << "[P2P] HOST: " << IP << " IS CONNECTED!";
    while (true) {
        std::cout << "\n\n################################\n" <<
                         "#                              #\n" <<
                         "#   1. List Remote Files       #\n" <<
                         "#   2. Pull (Download) a File  #\n" <<
                         "#   3. Push (Upload) a file    #\n" <<
                         "#   4. Disconnect from host    #\n" <<
                         "#                              #\n" <<
                         "################################\n" <<
                         "Choice: ";
        int input;
        std::cin >> input;
        if (input == 1) {
            //getLocalfiles();
            if (!sendMSG(CONsock, MessageType::LIST_REQ)) std::cout << "[P2P]: MESSAGE ERROR!\n";
            continue;
        }
        if(input == 4) {
            std::cout << "\n\ndisconnecting...";
            clientside.ExitService(CONsock);
            return 0;
        }
    }
}