# Dining Philosophers Problem - C++ Implementation

## Overview
This project is an implementation of the classic **Dining Philosophers Problem** using C++ threads (`std::thread`) and mutexes (`std::mutex`). The problem is a well-known synchronization challenge that demonstrates how to handle resource sharing among multiple processes in an operating system. This implementation was created as part of the **Operating Systems 2** course.

## Problem Description
The Dining Philosophers Problem involves five philosophers seated around a circular table, where each philosopher alternates between thinking and eating. A chopstick is placed between each pair of adjacent philosophers, and a philosopher must acquire both the left and right chopstick before eating. The challenge lies in ensuring that:
- Deadlock does not occur - all philosophers waiting indefinitely for chopsticks.
- Starvation does not occur - some philosophers never get to eat.

## Implementation details
### Key features:
- Multithreading using `std::thread`
- Mutex-based synchronization using `std::mutex`
- Randomized thinking & eating times (2000ms - 4000ms)
- Deadlock-free approach using an alternating chopstick acquisition orders strategy
- User-controlled termination via a separate thread listening for the SPACE key
  
### Thread structure:
1. `philosophers` - Each philosopher runs in a separate thread, logging their actions (thinking, eating, picking up chopsticks).
2. `keyListenerThread` - Listens for user input to terminate the simulation gracefully.

### Handling Critical Sections
- Each chopstick is represented as a `std::mutex`, so no chopstick can be simultaneously used by multiple philosophers.
- A philosopher must lock both adjacent chopstick mutexes before eating.
- Philosophers follow a specific chopstick acquisition order:
  - Even-indexed philosophers: Pick up left chopstick first, then right
  - Odd-indexed philosophers: Pick up right chopstick first, then left\
This prevents circular dependencies, eliminating deadlock.

## Project Structure
- `dining-philosophers.cpp`: Main source code for the project.
- `README.md`: Project documentation

## How to Run
Clone this repository:
```
git clone https://github.com/zuzdud/dining-philosophers-problem.git
cd dining-philosophers-problem
```
Compile the program:
```
g++ -o philosophers dining-philosophers.cpp
```
Run the executable:
```
./philosophers
```
**_Press SPACE to terminate the simulation._**

## Expected output
```
Philosopher 3 is thinking
Philosopher 4 is eating
Philosopher 1 is thinking
Philosopher 2 is eating
Philosopher 0 tries to pick up chopsticks 
Philosopher 4 is thinking
Philosopher 0 is eating
Philosopher 3 tries to pick up chopsticks
```
_The exact order may vary due to thread scheduling._

## References
[Dining Philosophers Problem - Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)

[std::mutex - CPP Reference](https://en.cppreference.com/w/cpp/thread/mutex)

[std::thread - CPP Reference](https://en.cppreference.com/w/cpp/thread/thread)
