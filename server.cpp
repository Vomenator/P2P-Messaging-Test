#include "common.hpp"

IP localIP;
bool exitprog = false;

int handlepeer(SOCKET sock, std::string peeraddr) {
    Message msg{};
    std::cout << "[P2P] PEER " << peeraddr << " CONNECTED";

    while (exitprog == false) {
        //if()
    }
    return 0;
}


int ServerEntry() {
    std::string exit;

    SOCKET OUTsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);       // this binds the rules to the socket itself configuring the rules
    if (OUTsocket == INVALID_SOCKET) {
        std::cout << "an Error has occured! INVALID SERVER SOCKET: 0x" << WSAGetLastError();
        WSACleanup();
        return -1;
    }
    bool opt = true;
    setsockopt(OUTsocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&opt), sizeof(opt));

    std::cout << "binding socket...";
    sockaddr_in localaddr{};
    localaddr.sin_family      = AF_INET;
    localaddr.sin_port        = htons(static_cast<u_short>(ForeignIP.port));

    if (bind(OUTsocket, reinterpret_cast<sockaddr*>(&localaddr), sizeof(localaddr)) == SOCKET_ERROR) {
        std::cout << "An error has occured! BINDING SOCKET: 0x" << WSAGetLastError();
        ExitService(OUTsocket);
        return -1;
    }
    std::cout << "\r\x1b[2K" << "success!";

    if(listen(OUTsocket, 10) == SOCKET_ERROR) {
        std::cout << "\r AN ERROR HAS OCCURED!, 0x" << WSAGetLastError();
        ExitService(OUTsocket);
        return -1;
    }
    system("cls");
    std::cout << "[P2P] LISTENING... ON PORT: " << ForeignIP.port;

    while (exitprog == false) {

        sockaddr_in clientaddr{};
        int sizepeer = sizeof(clientaddr);

        SOCKET client = accept(OUTsocket, reinterpret_cast<sockaddr*>(&clientaddr), &sizepeer);
        //SOCKET client = accept(OUTsocket, reinterpret_cast<sockaddr*>(&clientaddr), &sizepeer);
        if (client == INVALID_SOCKET) {
            std::cout << "AN ERROR HAS OCCURED!, 0x" << WSAGetLastError();
            continue;
        }
        else {
            char ip[INET_ADDRSTRLEN]{};
            InetNtopA(AF_INET, &clientaddr.sin_addr, ip, sizeof(ip));
            handlepeer(client, std::string(ip));
        }
    }
    std::cin >> exit;
    ExitService(OUTsocket);
    return 0;
}