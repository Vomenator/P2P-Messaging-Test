#include "common.hpp"


using namespace std;

//fs::path FILEFOLDER = "./shared";

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

    string IPaddress = input.substr(0,portpos);
    int port = stoi(input.substr(portpos+1));

    // filesystem init
        fs::create_directories(FILEFOLDER);

    if (ServerEntry(port) == -1) return 1;
    //if (ClientEntry(IPaddress, port) == -1) return 1;
    return 0;
}