#include "common.hpp"
#include <bitset>

bool exitprog = false;
Message recvMessage;
networkbase serverObject;

bool recvMSG(SOCKET sock, Message& message) {
    //char* msg;
    uint32_t len = 264;
    //serverObject.recievePacket(sock, &message, len);
    //if(len != networkConfigVAR::MSG_LEN) return false;
    return true;
}

int handlepeer(SOCKET sock, std::string peeraddr) {
    //peer.setupForeignAddr(peeraddr, NULL);
    Message msg = {};
    std::cout << "[P2P] PEER " << peeraddr << " CONNECTED";
    std::cout << std::dec << sizeof(msg) << "\n";

    while (true) {
        if (recvMSG(sock, msg) == true) {
            //std::cout << "hello world!111";
            std::cout << std::bitset<264>(*reinterpret_cast<char*>(&msg));
            std::cout << "\n";
            if (msg.type == MessageType::LIST_REQ) {
                std::cout << "hello world!";
            }
            if (msg.type == MessageType::LIST_ACK) {
                std::cout << "hamburger!";
            }
        }
    }
    return 0;
}


int ServerEntry(int port) {
    serverObject.configureNetwork(); // initalises global network variables
    serverObject.setuplocalAddr(port); // initalises local address
    SOCKET OUTsocket = serverObject.networkinitTCP(true, false); // initalises the server socket
    if (OUTsocket == INVALID_SOCKET) {
        std::cout << "ERROR INVALID SERVER SOCK!: 0x" << WSAGetLastError();
        serverObject.ExitService(OUTsocket);
        return -1;
    }
    std::string exit;
    
    bool opt = true;
    //setsockopt(OUTsocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&opt), sizeof(opt));

    while (exitprog == false) {

        sockaddr_in clientaddr{};
        int sizepeer = sizeof(clientaddr);

        SOCKET client = accept(OUTsocket, reinterpret_cast<sockaddr*>(&clientaddr), &sizepeer);
        if (client == INVALID_SOCKET) {
            std::cout << " AN ERROR HAS OCCURED!, 0x" << WSAGetLastError();
            continue;
        }
        else {
            std::cout << "newclient!";
            char ip[INET_ADDRSTRLEN]{};
            InetNtopA(AF_INET, &clientaddr.sin_addr, ip, sizeof(ip));
            std::thread(handlepeer, client, std::string(ip)).detach();
        }
        //std::cin >> exit;
        //if (exit == "exit") {
          //  exitprog = true;
        //}
    }
    serverObject.ExitService(OUTsocket);
    return 0;
}