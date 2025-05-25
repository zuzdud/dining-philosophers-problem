#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mutex>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 12345 // server port
#define BUFFER_SIZE 1024

bool isRunning = true;

std::vector<int> clients; // list of clients sockets
std::mutex clientsMutex;  // mutex for client list

void broadcastMessage(std::string message, int clientSocket)
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (int client_fd : clients)
    {
        if (client_fd != clientSocket)
        {
            if (send(client_fd, message.c_str(), message.size(), 0) == SOCKET_ERROR)
            {
                std::cerr << "Failed to send message to client " << client_fd << "\n";
            }
        }
    }
}
void handleClient(int clientSocket)
{
    char messageBuffer[BUFFER_SIZE];

    std::string welcomeMessage = "Well, hello there!\n";

    // to client socket send welcome message
    send(clientSocket, welcomeMessage.c_str(), welcomeMessage.length(), 0);

    while (isRunning)
    {
        int bytes_received = recv(clientSocket, messageBuffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0)
        {
            int err = WSAGetLastError();
            if (err == WSAETIMEDOUT)
                continue; // timeout, check isRunning again
            break;
        }
        messageBuffer[bytes_received] = '\0';
        std::string message = "Client " + std::to_string(clientSocket) + ": " + messageBuffer;
        std::cout << message;
        broadcastMessage(message, clientSocket);
    }

    // remove client on disconnect
    {
        // lock clients vector mutex
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(clients.begin() + clientId);
    }
    closesocket(clientSocket);
    std::cout << "Client disconnected\n";
}

void listenForCommands()
{
    std::string input;
    while (isRunning)
    {
        std::getline(std::cin, input);
        if (input == "/shutdown")
        {
            std::cout << "Shutting down server..." << std::endl;
            isRunning = false;
            break;
        }
    }
}

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

    std::thread commandsThread(listenForCommands);

    while (isRunning)
    {
        sockaddr_in clientAddr{};
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientSize);

        std::cout << "New client connected: " << clientSocket << std::endl;
        int clientId = clients.size();
        {
            // lock client vector to eliminate race condition
            std::lock_guard<std::mutex> lock(clientsMutex);

            // add client socket to vector
            clients.push_back(std::make_pair(clientSocket, clientId));
        }

        // new thread for client
        std::thread t(handleClient, clientSocket, clientId);
        t.detach(); // detach thread to handle client independently of this loop
    }

    commandsThread.join();
    std::cout << "Command thread finished. Closing all connections..." << std::endl;

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        for (int clientSocket : clients)
        {
            shutdown(clientSocket, SD_BOTH);
            closesocket(clientSocket);
        }
        clients.clear();
    }

    closesocket(serverSocket);

    WSACleanup();
    return 0;
}