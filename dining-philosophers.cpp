#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <random>
#include <conio.h>

int numberOfPhilosophers = 0;
std::vector<std::thread> philosophersThreads; // vector for storing philosophers threads
std::vector<std::mutex> chopsticksMutexes;    // each chopstick is a mutex
std::mutex coutMutex;                         // mutex controlling access to console
bool isRunning = true;                        // Flag to control philosopher execution

// generating random thinking and eating times in milliseconds
std::mt19937 rng{std::random_device{}()};
std::uniform_int_distribution<int> actionTime{2000, 4000};

void eat(const int philosopherId, const int leftChopstick, const int rightChopstick)
{
    if (isRunning)
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
}

void think(const int philosopherId)
{
    if (isRunning)
    {
        {
            std::unique_lock<std::mutex> lock(coutMutex);
            std::cout << "Philosopher " << philosopherId << " is thinking" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(actionTime(rng)));
    }
}

// func that will be run by each philosopher thread
void philosopherRoutine(const int philosopherId)
{

    int leftChopstick = philosopherId;
    int rightChopstick = (philosopherId + 1) % numberOfPhilosophers;

    // enforcing different chopstick acquisition orders
    if (philosopherId % 2 == 1)
        std::swap(leftChopstick, rightChopstick);

    // main loop of each philosopher
    while (isRunning)
    {
        think(philosopherId);
        eat(philosopherId, leftChopstick, rightChopstick);
    }

    {
        std::unique_lock<std::mutex> lock(coutMutex);
        std::cout << "Philosopher " << philosopherId << " has stopped.\n";
    }
}

void listenForKeyPress()
{
    while (isRunning)
    {
        if (_kbhit() && _getch() == ' ') // if space key is pressed
        {
            isRunning = false; // stop all philosopher threads
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cout << "\nUser pressed space. Stopping simulation...\n";
            }
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main()
{
    std::cout << "Dining Philosophers Problem lets gooo" << std::endl
              << std::endl;

    std::cout << "\nPress SPACE to end simulation" << std::endl;
    
    // ensure that there are at least two philosophers
    while (numberOfPhilosophers < 2)
    {
        std::cout << "How many philosophers are at the table? (at least two)" << std::endl;
        std::cin >> numberOfPhilosophers;
    }

    // start the key listener thread
    std::thread keyListenerThread(listenForKeyPress);

    // create mutexes
    chopsticksMutexes = std::vector<std::mutex>(numberOfPhilosophers);

    // create threads
    for (int i = 0; i < numberOfPhilosophers; i++)
    {
        philosophersThreads.emplace_back(philosopherRoutine, i);
    }

    // wait for philosopher threads to finish
    for (auto &philosopher : philosophersThreads)
    {
        philosopher.join();
    }

    // wait for key listener thread to finish
    keyListenerThread.join();

    std::cout << "Simulation finished.\n";
    return 0;
}