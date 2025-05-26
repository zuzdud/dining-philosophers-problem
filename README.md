# Operating Systems 2 – C++ Projects

## Overview
This repository contains two multithreaded C++ projects developed as part of the **Operating Systems 2** course. Both projects demonstrate key concepts in concurrent programming, thread synchronization, and resource management using modern C++.

## 📁 `chat-server/`

A multithreaded chat server and client application using TCP sockets (WinSock2 API).
- Handles multiple clients concurrently using `std::thread`.
- Synchronizes access to shared data (client list) using `std::mutex`.
- Features real-time message broadcasting and graceful shutdown support.
- Includes client support for message sending and receiving in parallel.

## 📁 `dining-philosophers-problem/`

An implementation of the classic Dining Philosophers Problem using threads and mutexes.
- Prevents deadlock and starvation through an ordered resource acquisition strategy.
- Simulates randomized thinking and eating times.
- Controlled termination via keyboard input listener.

## Technologies Used
- C++17
- `std::thread`, `std::mutex`
- WinSock2 API (for chat server)

## How to Run

Clone this repository:
```
git clone https://github.com/zuzdud/operating-systems-2-projects.git
```
Go to desired project directory:
```
cd dining-philosophers-problem
cd chat-server
```
Each project contains its own source files and build instructions.
See the individual directories for compilation and execution steps.