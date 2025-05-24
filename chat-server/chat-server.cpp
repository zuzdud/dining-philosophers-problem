#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 12345 // server port

int main()
{
    std::cout << "Chat Server lets gooo" << std::endl
              << std::endl;

    // WinSock lib initialization, version 2.2, data in wsaData
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // create socket, AF_INET is IPv4, SOCK_STREAM is TCP, 0 default protocol for TCP
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; // accept connections from any IP

    // bind ip addres and port to server socket
    bind(serverSocket, (sockaddr *)&server_addr, sizeof(server_addr));

    // listen on this port
    // max 10 connections
    listen(serverSocket, 10);

    std::cout << "Server started on port " << PORT << "\n";

    while (true)
    {
        sockaddr_in client_addr{};
        socklen_t client_size = sizeof(client_addr);
        int client_socket = accept(serverSocket, (sockaddr *)&client_addr, &client_size);

        std::cout << "New client connected: " << client_socket << "\n";
    }

    closesocket(serverSocket);

    WSACleanup();
    return 0;
}