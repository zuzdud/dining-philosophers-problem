#include <iostream>
#include <thread>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 12345
#define BUFFER_SIZE 1024

bool isRunning = true;

void receiveMessages(int sockfd)
{
    char messageBuffer[BUFFER_SIZE];
    while (isRunning)
    {
        int bytes_received = recv(sockfd, messageBuffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0)
        {
            std::cout << "Disconnected from server.\n";
            isRunning = false;
            break;
        }
        messageBuffer[bytes_received] = '\0';
        std::cout << messageBuffer;
        std::string msg(messageBuffer);

        if (msg == "SERVER: shutting down\n")
        {
            isRunning = false;
            shutdown(sockfd, SD_BOTH);
            closesocket(sockfd);
            break;
        }
    }
}

void inputThreadFunc(int sock)
{
    std::string input;
    while (isRunning)
    {
        if (!std::getline(std::cin, input))
        {
            isRunning = false;
            break;
        }
        if (input == "/quit")
        {
            isRunning = false;
            shutdown(sock, SD_BOTH);
            closesocket(sock);
            break;
        }
        send(sock, input.c_str(), static_cast<int>(input.length()), 0);
    }
}

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

    std::thread recvThread(receiveMessages, sock);
    std::thread inputThread(inputThreadFunc, sock);

    while (isRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (recvThread.joinable())
        recvThread.join();
    if (inputThread.joinable())
        inputThread.join();

    closesocket(sock);
    WSACleanup();
    return 0;
}