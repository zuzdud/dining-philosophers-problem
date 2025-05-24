#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 12345 // server port

std::vector<int> clients; // list of clients sockets
std::mutex clientsMutex; // mutex for client list

void handleClient(int clientSocket);

int main()
{
    std::cout << "Chat Server lets gooo" << std::endl
              << std::endl;

    // WinSock lib initialization, version 2.2, data in wsaData
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // create socket, AF_INET is IPv4, SOCK_STREAM is TCP, 0 default protocol for TCP
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY; // accept connections from any IP

    // bind ip addres and port to server socket
    bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));

    // listen on this port
    // max 10 connections
    listen(serverSocket, 10);

    std::cout << "Server started on port " << PORT << std::endl;

    while (true)
    {
        sockaddr_in clientAddr{};
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientSize);

        std::cout << "New client connected: " << clientSocket << std::endl;

        {
            // lock client vector to eliminate race condition 
            std::lock_guard<std::mutex> lock(clientsMutex);
            // add client socket to vector
            clients.push_back(clientSocket);
        }

        // new thread for client
        std::thread t(handleClient, clientSocket);
        t.detach();  // detach thread to handle client independently 
    }

    closesocket(serverSocket);

    WSACleanup();
    return 0;
}