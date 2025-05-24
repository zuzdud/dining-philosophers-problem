#include <iostream>
#include <thread>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 12345
#define BUFFER_SIZE 1024

int main()
{

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Connection failed.\n";
        return 1;
    }

    std::cout << "Connected to server.\n";

    WSACleanup();
    return 0;
}