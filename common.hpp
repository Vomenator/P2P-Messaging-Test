#pragma once

#include "networkingbasic_lib/windows/windowsINET.h"
//#include <thread>

//namespace fs = std::filesystem;

extern networkbase serverObject;
extern Message sendMessage;



inline bool sendMSG(SOCKET sock, MessageType type, const std::string& filename = "") {
    Message newMessage;
    newMessage.type = type;
    std::cout<<"hello";
    //newMessage->length = files.size();
    
    // data restructure, convers the vector<string> into a char array;
    if (!filename.empty()) {
        //strncpy_s(sendMessage.MessageData.assign(filename.begin(), filename.end()).data(), sizeof(sendMessage.MessageData), filename.c_str(), _TRUNCATE);
        newMessage.MessageData.assign(filename.c_str(), filename.c_str() + filename.size() + 1); // this adds the charactor to each index, and makes sure to add the null terminator at the end
    }
    if (serverObject.sendPacket(sock, reinterpret_cast<const char*>(&newMessage), sizeof(newMessage))) {
        std::cerr << "Failed to send message." << std::endl;
        return false;
    }
    
    return true;
}

// FORWARD DECLARATIONS

    // SERVER.CPP
int ServerEntry(int port);

    // CLIENT.CPP
int ClientEntry(std::string IP, int port);