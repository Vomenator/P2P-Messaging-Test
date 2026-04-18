#pragma once
// custom configuration file
#include "../configuration.h"

//namespace fs = std::filesystem;

enum class Psize {
    BYTE = 1,
    KB = 1024,
    MB = 1024 * KB,
    GB = 1024 * MB
};

// default global variables 
class networkConfigVAR {
    public:
        static int MAX_PEERS;
        //static int PORT;
        static int MSG_LEN;
        static int PAYLOAD_LEN;

        static inline struct sockaddr_in ForeignAddr = {0};
        static inline struct sockaddr_in localAddr = {0};

        static bool isconfigured() {
            return (MAX_PEERS != 0 && MSG_LEN != 0 && PAYLOAD_LEN != 0);
        }
    private:
        bool configured = false;
};

// stopping it from whining about uninitalised values
inline int networkConfigVAR::MAX_PEERS = 0;
//inline int networkConfigVAR::PORT = 0;
inline int networkConfigVAR::MSG_LEN = 0;
inline int networkConfigVAR::PAYLOAD_LEN = 0;

//static_assert(sizeof(Message) == networkConfigVAR::MSG_LEN, "Message must match MSG_LEN bytes");

class networkbase {
    public:
        inline void configureNetwork(int maxPeers = 10, int msgLen = 256, int payloadLen = 1024, Psize sizeUnit = Psize::KB);
        inline void setupForeignAddr(std::string IP, int port);
        inline void setuplocalAddr(int port);
        inline SOCKET networkinitTCP(bool isServer, bool SKIPWSA = true);
        inline bool sendPacket(SOCKET sock, const char* msg, size_t len);
        inline bool recievePacket(SOCKET sock, void* msg, size_t len);
        inline void ExitService(SOCKET sock);
    private:
        inline bool checkMessageSize(const Message& msg);
};
inline void networkbase::ExitService(SOCKET sock) {
    closesocket(sock);
    WSACleanup();
}

inline void networkbase::configureNetwork(int maxPeers, int msgLen, int payloadLen, Psize sizeUnit) {
    try {
        if (maxPeers <= 0 || msgLen <= 0 || payloadLen <= 0) {
            throw std::invalid_argument("Configuration values must be greater than zero.");
        }
        networkConfigVAR::MAX_PEERS = maxPeers;
        networkConfigVAR::MSG_LEN = msgLen;
        networkConfigVAR::PAYLOAD_LEN = payloadLen * static_cast<int>(sizeUnit);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in configureNetwork: " << e.what() << std::endl;
    }
}

inline void networkbase::setupForeignAddr(std::string IP, int port) {
    try {
        if (IP.empty()) {
            throw std::invalid_argument("IP address cannot be empty.");
        }
        if (port <= 0) {
            throw std::invalid_argument("Port number must be greater than zero.");
        }
        networkConfigVAR config;
        config.ForeignAddr.sin_family = AF_INET;
        config.ForeignAddr.sin_port = htons(static_cast<u_short>(port));
        InetPtonA(AF_INET, IP.c_str(), &config.ForeignAddr.sin_addr.s_addr);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in setupForeignAddr: " << e.what() << std::endl;
        return;
    }
}

inline void networkbase::setuplocalAddr(int port) {
    try {
        if (port <= 0) {
            throw std::invalid_argument("Port number must be greater than zero.");
        }
        networkConfigVAR config;
        config.localAddr.sin_family = AF_INET;
        config.localAddr.sin_port = htons(static_cast<u_short>(port));
        config.localAddr.sin_addr.s_addr = INADDR_ANY;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in setuplocalAddr: " << e.what() << std::endl;
        return;
    }
}

inline bool networkbase::checkMessageSize(const Message& msg) {
    if (msg.MessageData.size() > networkConfigVAR::MSG_LEN || msg.MessageData.size() < networkConfigVAR::MSG_LEN) {
        std::cerr << "Message data DOES NOT match expected length." << std::endl;
        return false;
    }
    return true;
}

SOCKET networkbase::networkinitTCP(bool isServer, bool SKIPWSA) {
    try {

        if (networkConfigVAR::isconfigured() == false) {
            throw std::runtime_error("Network configuration is not set. Please call configureNetwork() before initializing the network.");
        }
        if (isServer == true) {
            if (networkConfigVAR::localAddr.sin_port == 0) {
                throw std::runtime_error("Local address is not set. Please call setuplocalAddr() before initializing the server.");
            }
        }
        else {
            if (networkConfigVAR::ForeignAddr.sin_port == 0 || networkConfigVAR::ForeignAddr.sin_addr.s_addr == 0) {
                throw std::runtime_error("Foreign address is not set. Please call setupForeignAddr() before initializing the client.");
            }
        }

        if (SKIPWSA == false) {
            WSADATA wsaData;
            int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (result != 0) {
                WSACleanup();
                std::cerr << "WSAStartup failed: " << result << std::endl;
                throw std::runtime_error("Failed to initialize Winsock: " + std::to_string(WSAGetLastError()));
            }
        }
        SOCKET sockie(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
        if (sockie == INVALID_SOCKET) {
            ExitService(sockie);
            throw std::runtime_error("Failed to create socket: " + std::to_string(WSAGetLastError()));
        }
        bool opt = true;
        setsockopt(sockie, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&opt), sizeof(opt));
        
        if (isServer == true) {

            if (bind(sockie, reinterpret_cast<sockaddr*>(&networkConfigVAR().localAddr), sizeof(networkConfigVAR().localAddr)) == SOCKET_ERROR) {
                ExitService(sockie);
                throw std::runtime_error("Bind failed: " + std::to_string(WSAGetLastError()));
            }
            if (listen(sockie, networkConfigVAR().MAX_PEERS) == SOCKET_ERROR) {
                ExitService(sockie);
                throw std::runtime_error("Listen failed: " + std::to_string(WSAGetLastError()));
            }
        }
        else {
            if (connect(sockie, reinterpret_cast<sockaddr*>(&networkConfigVAR().ForeignAddr), sizeof(networkConfigVAR().ForeignAddr)) == SOCKET_ERROR) {
                ExitService(sockie);
                throw std::runtime_error("Connect failed: " + std::to_string(WSAGetLastError()));
            }
        }
        return sockie;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in networkinitTCP: " << e.what() << std::endl;
        return false;
    }
}
 
inline bool networkbase::sendPacket(SOCKET sock, const char* msg, size_t len) {
    try {
        if (networkConfigVAR::ForeignAddr.sin_addr.s_addr == 0) {
            throw std::runtime_error("Please configure the foreign address using setupForeignAddr() before sending messages.");
        }
        int totalSent = 0;
        while (totalSent < (networkConfigVAR().MSG_LEN - 9)) {
            int sent = send(sock, msg + totalSent, networkConfigVAR().MSG_LEN - totalSent, 0);
            if (sent == SOCKET_ERROR) {
                std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
                throw std::runtime_error("Failed to send message");
            }
            totalSent += sent;
        }
            return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in sendMessage: " << e.what() << std::endl;
        return false;
    }
}

inline bool networkbase::recievePacket(SOCKET sock, void* msg, size_t len) {
    char* temp = static_cast<char*>(msg);
    int totalSent = 0;
    while (len > 0) {
        int recieved = recv(sock, temp, static_cast<int>(len), 0);
        std::cout << recieved << " " << temp << "\n";
        if (totalSent <= 0) return false;
        temp += recieved;
        len -= recieved;
    msg = temp;
    return true;
}
