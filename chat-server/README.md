# Multithreaded Chat Server - C++ Implementation

## Overview
This project is a multithreaded **TCP Chat Server** implemented in C++ using Windows Sockets (WinSock), `std::thread`, and `std::mutex`. It was developed as part of the **Operating Systems 2** course to explore network communication, concurrency, and thread-safe resource sharing.

## Problem Description
The goal is to simulate a basic chat server that allows multiple clients to connect, exchange messages in real time, and handle server-side input for graceful shutdown. Each connected client can send and receive messages broadcast to all other participants, emulating a group chat room.
The server must ensure:
- Multiple clients can communicate simultaneously without message corruption or delay.
- Thread safety when accessing shared resources (e.g., list of client sockets).
- Clean termination of client connections and server threads.

## Implementation details
### Key features:
- TCP-based communication over IPv4 using the WinSock2 API
- Support for multiple clients using `std::thread`
- Thread-safe broadcast using `std::mutex`
- Graceful server shutdown via user input (`/shutdown`)
- Graceful client exit via user command (`/quit`)
  
### Thread structure:
**Server Side**
1. **Client Threads** – Each client is handled in its own thread for concurrent message handling.
2. **Command Listener Thread** – Listens for server-side input (e.g., `/shutdown`) to stop the server and notify all clients.
**Client Side**
1. **Receive Thread** – Continuously receives messages from the server (`receiveMessages`) and displays them to the user.
2. **Input Thread** – Captures user input (`inputThreadFunc`) and sends it to the server. Also handles the `/quit` command to disconnect gracefully.

### Synchronization & Thread Safety:
- The list of active client sockets is protected using a `std::mutex`.
- Atomic flag (`std::atomic<bool> isRunning`) ensures all threads are aware of shutdown state.
- All broadcasting and cleanup logic is safely enclosed in critical sections.

### Communication Protocol:
- Clients send plaintext messages.
- Server broadcasts each message to all clients (except the sender).
- A welcome message is sent to each client upon connection.
- Special shutdown messages notify clients when the server is shutting down.
- When client disconnets, a message is broadcasted to all clients.

## Project Structure
- `chat-server.cpp` – Main server implementation.
- `chat-client.cpp` – Console-based client implementation.
- `README.md` – Project documentation.

## How to Run
Compile the server and client:
```
g++ -o server chat-server.cpp -lws2_32
g++ -o client chat-client.cpp -lws2_32
```
Start the server:
```
./server
```
Start one or more clients (in separate terminals):
```
./client
```
Interact:
- Clients type and send messages.
- Server broadcasts messages to all connected clients.
- Type `/quit` in a client to disconnect.
- Type `/shutdown` in the server console to shut down the server.

## Expected output
Server:
```
Chat Server lets gooo

Server started on port 12345
New client connected: 456
Client 456: Hello everyone!
Client 789 disconnected
```
Client:
```
Connected to server.
Well, hello there!
Client 456: Hello everyone!
Client 789: Hi!
SERVER: shutting down
Disconnected from server.
```
_Actual output will vary depending on user input and network scheduling._

## References
[WinSock2 API Documentation](https://learn.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)

[std::mutex - CPP Reference](https://en.cppreference.com/w/cpp/thread/mutex)

[std::thread - CPP Reference](https://en.cppreference.com/w/cpp/thread/thread)
