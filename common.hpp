#pragma once
#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <cstdint>
#include <vector>

namespace fs = std::filesystem;

inline constexpr int MSG_LEN = 256;
inline constexpr int PAYLOADLENGTH = 1024;

enum class MSGTYPE : unsigned char {
    SEND_ACK = 0,
    LIST_REQ = 1,
    LIST_ACK = 2,
    GET_FILE = 3,
    SEND_FILE = 4,
    FILE_BYTE = 5,
    ISSUE = 6
};

struct IP{
    std::string IPaddress;
    int         port;
};

#pragma pack(push, 1)
struct Message {
    MSGTYPE            type;
    unsigned long long payload_len;
    char               filename[247];
};
#pragma pack(pop)
static_assert(sizeof(Message) == MSG_LEN, "Header must be exactly 256 bytes");        // gets final size of the header

extern IP ForeignIP;


inline void ExitService(SOCKET sock) {
    closesocket(sock);
    WSACleanup();
}

bool send_all(SOCKET sock, const char* data, int length, int flags) {
    int totalSent = 0;
    int bytesLeft = length;
    const char* dataPtr = data;

    while (totalSent < sizeof(sendMSG)) {
        int sent = send(sock, dataPtr + totalSent, bytesLeft, 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "Error sending message: " << WSAGetLastError() << std::endl;
            return false; // Handle error appropriately
        }
        totalSent += sent;
        bytesLeft -= sent;
    }
    return true; // Message sent successfully
}

void getLocalfiles(SOCKET sock) {
    std::vector<std::string> files = {};
    int i = 0;
    fs::path stupidredef = "./shared";
    for (const auto& file : fs::directory_iterator(stupidredef)) {
        std::cout << "["<< i << "]: "<< file.path().filename().string() << "\n";
        files.push_back(file.path().filename().string());
        i++;
    }
    //uint64_t length = sizeof(files);
    std::cout << "done\n";
    sendMSG(sock, Message{MSGTYPE::LIST_ACK});
}

inline void sendMSG(SOCKET sock, Message type, const std::string& filename = "") {
    Message sendMSG {};

    sendMSG.type = type.type;
    sendMSG.payload_len = files.size();
    
    // data restructure, convers the vector<string> into a char array;
    if (!filename.empty()) {
        strncpy_s(sendMSG.filename, sizeof(sendMSG.filename), filename.c_str(), _TRUNCATE);
    }
    if (!send_all(sock, reinterpret_cast<const char*>(&sendMSG), sizeof(sendMSG), 0)) {
        std::cerr << "Failed to send message." << std::endl;
    }
}

// FORWARD DECLARATIONS

    // SERVER.CPP
int ServerEntry();

    // CLIENT.CPP
int ClientEntry(std::string& IP, int port);