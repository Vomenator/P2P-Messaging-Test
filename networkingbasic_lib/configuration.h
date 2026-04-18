#pragma once

// windows standard libaries
#include <winsock2.h>                   // network libaries
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") 

#include <iostream>                     // other standard libaries
#include <string>
#include <vector>
#include <mutex>
#include <filesystem>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>

enum class MessageType : unsigned char {
    //SEND_ACK = 0,
    LIST_REQ = 1,
    LIST_ACK = 2,
    GET_FILE = 3,
    SEND_FILE = 4,
    FILE_BYTE = 5,
    ISSUE = 6
};

#pragma pack(push, 1)
struct Message {
    MessageType type;
    uint64_t length;
    std::vector<char> MessageData; // 9 is the size of the other values i.e length and type
};
#pragma pack(pop)