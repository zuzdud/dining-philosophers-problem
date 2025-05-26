#include <iostream>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mutex>
#include <string>
#include <algorithm>
#include <thread>
#include <atomic>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 12345 // server port
#define BUFFER_SIZE 1024

std::atomic<bool> isRunning(true);

std::vector<int> clients; // list of clients sockets
std::vector<std::thread> clientThreads;
std::mutex clientsMutex; // mutex for client list

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
    send(clientSocket, welcomeMessage.c_str(), static_cast<int>(welcomeMessage.length()), 0);

    while (isRunning)
    {
        int bytes_received = recv(clientSocket, messageBuffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0)
        {
            break;
        }
        messageBuffer[bytes_received] = '\0';
        std::string message = "Client " + std::to_string(clientSocket) + ": " + messageBuffer + "\n";
        std::cout << message;
        broadcastMessage(message, clientSocket);
    }

    // remove client on disconnect
    {
        // lock clients vector mutex
        std::lock_guard<std::mutex> lock(clientsMutex);
        auto it = std::find(clients.begin(), clients.end(), clientSocket);
        if (it != clients.end())
        {
            clients.erase(it);
        }
    }
    closesocket(clientSocket);
    std::cout << "Client " << std::to_string(clientSocket) << " disconnected\n";
    broadcastMessage("Client " + std::to_string(clientSocket) + " disconnected\n", -1);
}

void closeAllClients()
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (int clientSocket : clients)
    {
        shutdown(clientSocket, SD_BOTH);
        closesocket(clientSocket);
    }
    clients.clear();
}

void listenForCommands(int serverSocket)
{
    std::string input;
    while (isRunning)
    {
        std::getline(std::cin, input);
        if (input == "/shutdown")
        {
            std::cout << "Shutting down server..." << std::endl;
            isRunning = false;
            broadcastMessage("SERVER: shutting down\n", -1);
            closesocket(serverSocket);
            closeAllClients();

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
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }
    // create socket, AF_INET is IPv4, SOCK_STREAM is TCP, 0 default protocol for TCP
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY; // accept connections from any IP

    // bind ip addres and port to server socket
    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << "\n";
        return 1;
    }

    // listen on this port
    // max 10 connections
    if (listen(serverSocket, 10) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << "\n";
        return 1;
    }

    std::cout << "Server started on port " << PORT << std::endl;

    std::thread commandsThread(listenForCommands, serverSocket);

    while (isRunning)
    {
        sockaddr_in clientAddr{};
        int clientSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientSize);

        if (clientSocket == INVALID_SOCKET)
        {
            if (!isRunning)
            {
                // Server is shutting down and socket closed -> break loop
                break;
            }
            std::cerr << "Accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        std::cout << "New client connected: " << clientSocket << std::endl;
        {
            // lock client vector to eliminate race condition
            std::lock_guard<std::mutex> lock(clientsMutex);

            // add client socket to vector
            clients.push_back(clientSocket);
        }

        clientThreads.emplace_back(handleClient, clientSocket);
    }

    closeAllClients();

    for (auto &t : clientThreads)
    {
        if (t.joinable())
            t.join();
    }

    commandsThread.join();

    WSACleanup();

    return 0;
}