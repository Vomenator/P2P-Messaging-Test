#include "common.hpp"


bool exitProg = false;
std::vector<std::string> files = {};

int ClientEntry(std::string& IP, int port) {
    SOCKET CONsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (CONsock == INVALID_SOCKET) {
        std::cout << "ERROR INVALID CLIENT SOCK!: 0x" << WSAGetLastError();
        WSACleanup();
        return -1;
    }
    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(static_cast<u_short>(ForeignIP.port));
    InetPtonA(AF_INET, IP.c_str(), &addr.sin_addr);

    if(connect(CONsock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "ERROR CLIENT CONNECTION HAS FAILED: 0x" << WSAGetLastError();
        ExitService(CONsock);
        return -1;
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
            getLocalfiles();
        }
        if(input == 4) {
            std::cout << "\n\ndisconnecting...";
            ExitService(CONsock);
            return 0;
        }
    }
}