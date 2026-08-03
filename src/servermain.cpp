#include <cstring>
#include <algorithm>

#include "directory.hpp" 

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // tell Visual Studio to link the network library
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>

// create Windows-like macros so Linux code looks identical
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif
// -----------------------------------

using namespace std;

#define PORT "8080"
#define MAX_BUFFER 1024

int main() {
    Directory myDatabase;
    Parser myParser;

    // Windows specific network startup (Ignored on Linux)
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed.\n";
        return 1;
    }
#endif

    // setup the listening socket
    struct addrinfo hints, * ai;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &ai) != 0) {
        cerr << "getaddrinfo failed.\n";
        return 1;
    }

    SOCKET listener = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (listener == INVALID_SOCKET) {
        cerr << "socket failed.\n";
        return 1;
    }

    // skip "Address already in use" errors 
    int yes = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int));

    if (::bind(listener, ai->ai_addr, ai->ai_addrlen) == SOCKET_ERROR) {
        cerr << "bind failed.\n";
        return 1;
    }
	freeaddrinfo(ai); // delete redundant address info

    if (listen(listener, 10) == SOCKET_ERROR) {
        cerr << "listen failed.\n";
        return 1;
    }

    cout << "Server is running! Listening on port " << PORT << "...\n";

    // setup poll() 
    // This vector tracks ALL connected clients.
    vector<struct pollfd> pfds;

    // Add the listener socket to our poll array first
    struct pollfd pfd;
    pfd.fd = listener;
    pfd.events = POLLIN; // We want to know when it is ready to READ (new connection)
    pfds.push_back(pfd);

	// main server loop
    while (true) {

        // poll() pauses the program until AT LEAST ONE socket has data ready
#ifdef _WIN32
        int poll_count = WSAPoll(pfds.data(), pfds.size(), -1);
#else
        int poll_count = poll(pfds.data(), pfds.size(), -1);
#endif

        if (poll_count == SOCKET_ERROR) {
            cerr << "poll failed.\n";
            break;
        }

        for (int i = 0; i < (int)pfds.size(); i++) {

            // check if POLLIN
            if (pfds[i].revents & POLLIN) {

                // listener is ready
                if (pfds[i].fd == listener) {
                    struct sockaddr_storage remoteaddr;
                    socklen_t addrlen = sizeof remoteaddr;

                    SOCKET newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
                    if (newfd == INVALID_SOCKET) {
                        cerr << "accept failed.\n";
                    }
                    else {
                        // Add the new client to our poll array
                        struct pollfd new_pfd;
                        new_pfd.fd = newfd;
                        new_pfd.events = POLLIN;
                        pfds.push_back(new_pfd);
                        cout << "New client connected! Socket ID: " << newfd << "\n";
                    }
                }
                // the command has been sent
                else {
                    char buffer[MAX_BUFFER];
                    memset(buffer, 0, MAX_BUFFER);
                    int bytes_received = recv(pfds[i].fd, buffer, MAX_BUFFER - 1, 0);

                    // client disconnected or error
                    if (bytes_received <= 0) {
                        cout << "Client disconnected. Socket ID: " << pfds[i].fd << "\n";
                        closesocket(pfds[i].fd);

                        pfds.erase(pfds.begin() + i);
                        i--;
                    }
                    // received a command!
                    else {
						// bytes to string
                        string network_input(buffer);

                        // parse
                        vector<string> args = myParser.parse(network_input);

                        if (!args.empty()) {
                            // execute
                            string result = myDatabase.execute(args);

                            // send back the res
                            result += "\n"; // Add a newline so their terminal looks nice
                            send(pfds[i].fd, result.c_str(), result.length(), 0);
                        }

                        myDatabase.debugPrint(); // debug print to server
                    }
                }
            } // pollin
        } // loop
    } // while loop

    return 0;
}