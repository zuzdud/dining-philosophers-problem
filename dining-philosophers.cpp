#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <random>

int numberOfPhilosophers = 0;
std::vector<std::thread> philosophersThreads; // vector for storing philosophers threads
std::vector<std::mutex> chopsticksMutexes;    // each chopstick is a mutex
std::mutex coutMutex;                         // mutex controlling access to console

// generating random thinking and eating times in milliseconds
std::mt19937 rng{std::random_device{}()};
std::uniform_int_distribution<int> actionTime{2000, 4000};

void eat(const int philosopherId, const int leftChopstick, const int rightChopstick)
{
    {
        std::unique_lock<std::mutex> lock(coutMutex);
        std::cout << "Philosopher " << philosopherId << " tries to pick up chopsticks " << std::endl;
    }

    // locking mutexes = picking up chopsticks one by one
    chopsticksMutexes[leftChopstick].lock();
    chopsticksMutexes[rightChopstick].lock();

    {
        std::unique_lock<std::mutex> lock(coutMutex);
        std::cout << "Philosopher " << philosopherId << " is eating" << std::endl;
    }

    // mutexes are locked for the amount of time that philosopher is eating
    std::this_thread::sleep_for(std::chrono::milliseconds(actionTime(rng)));

    // unlocking mutexes
    chopsticksMutexes[leftChopstick].unlock();
    chopsticksMutexes[rightChopstick].unlock();
}

void think(const int philosopherId)
{
    {
        std::unique_lock<std::mutex> lock(coutMutex);
        std::cout << "Philosopher " << philosopherId << " is thinking" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(actionTime(rng)));
}

// func that will be run by each thread
void philosopherRoutine(const int philosopherId)
{
    
    int leftChopstick = philosopherId;
    int rightChopstick = (philosopherId + 1) % numberOfPhilosophers;

    // enforcing different chopstick acquisition orders
    if (philosopherId % 2 == 1)
        std::swap(leftChopstick, rightChopstick);

    // main loop of each philosopher
    while (true)
    {
        think(philosopherId);
        eat(philosopherId, leftChopstick, rightChopstick);
    }
}

int main()
{
    std::cout << "Dining Philosophers Problem lets gooo" << std::endl
              << std::endl;

    // ensure that there are at least two philosophers
    while (numberOfPhilosophers < 2)
    {
        std::cout << "How many philosophers are at the table? (at least two)" << std::endl;
        std::cin >> numberOfPhilosophers;
    }

    // create mutexes
    chopsticksMutexes = std::vector<std::mutex>(numberOfPhilosophers);

    // create threads
    for (int i = 0; i < numberOfPhilosophers; i++)
    {
        philosophersThreads.emplace_back(philosopherRoutine, i);
    }

    for (auto &philosopher : philosophersThreads)
    {
        philosopher.join();
    }
}