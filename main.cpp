#include "common.hpp"

using namespace std;

IP ForeignIP;

fs::path FILEFOLDER = "./shared";
//string IP = "";
//string port = "";

int winsockinit() {
    WSADATA winsockData;
    if(WSAStartup(MAKEWORD(2,2), &winsockData) != 0) {  // this binds the socket table to the variable and sets the socket rule version
        std::cout << "an Error has occured with the WSAstartup: 0x" << WSAGetLastError();
        return 0;
    }
    return 1;
}

int main() {
    int portpos = 0;
    string input;
    cin >> input;
    portpos = input.find(':');
    if (portpos == string::npos) {
        cout << "error expected format is: 192.168.255.255:0000\n";
        cout << "                         semicolon here  ^\n";
        return -1;
    }
    if (!winsockinit()) return -1; // initalises Win sock address table

    ForeignIP.IPaddress = input.substr(0,portpos);
    ForeignIP.port = stoi(input.substr(portpos+1));

    // filesystem init
        fs::create_directories(FILEFOLDER);

    //if (ServerEntry() == -1) return 1;
    if (ClientEntry(ForeignIP.IPaddress, ForeignIP.port) == -1) return 1;
    return 0;
}