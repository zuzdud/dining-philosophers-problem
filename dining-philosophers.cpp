#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <random>

int numberOfPhilosophers;
std::vector<std::thread> philosophersThreads; // vector for storing philosophers threads
std::vector<std::mutex> chopsticksMutexes;
std::mutex consolePrintingMutex;

std::mt19937 rng{std::random_device{}()};
std::uniform_int_distribution<int> thinkingTime{500, 2000};
std::uniform_int_distribution<int> eatingTime{300, 1000};


void eat(const int philosopherId, const int leftChopstick, const int rightChopstick)
{
    int currentEatingTime = eatingTime(rng);

    chopsticksMutexes[leftChopstick].lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    chopsticksMutexes[rightChopstick].lock();

    {
        std::unique_lock<std::mutex> lock(consolePrintingMutex);
        std::cout << "Philosopher " << philosopherId << " will be eating for " << currentEatingTime << "ms" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(currentEatingTime));

    {
        std::unique_lock<std::mutex> lock(consolePrintingMutex);
        std::cout << "Philosopher " << philosopherId << " stopped eating..." << std::endl;
    }
    chopsticksMutexes[leftChopstick].unlock();
    chopsticksMutexes[rightChopstick].unlock();
}

void think(const int philosopherId)
{
    int currentThinkingTime = thinkingTime(rng);
    {
        std::unique_lock<std::mutex> lock(consolePrintingMutex);
        std::cout << "Philosopher " << philosopherId << " will be thinking for " << currentThinkingTime << "ms" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(currentThinkingTime));

    {
        std::unique_lock<std::mutex> lock(consolePrintingMutex);
        std::cout << "Philosopher " << philosopherId << " stopped thinking..." << std::endl;
    }
}

// func that will be run by each thread
void runPhilosLife(const int philosopherId)
{
    int leftChopstick, rightChopstick;

    if(philosopherId%2==0){
        leftChopstick = philosopherId;
        rightChopstick = (philosopherId + 1) % numberOfPhilosophers;
    } else{
        leftChopstick = (philosopherId + 1) % numberOfPhilosophers;
        rightChopstick = philosopherId;
    }

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
    std::cout << "How many philosophers are at the table?" << std::endl;
    std::cin >> numberOfPhilosophers;

    // create threads
    for (int i = 0; i < numberOfPhilosophers; i++)
    {
        philosophersThreads.emplace_back(runPhilosLife, i);
    }

    chopsticksMutexes = std::vector<std::mutex>(numberOfPhilosophers);

    // wait for them to end
    for (auto &philosopher : philosophersThreads)
    {
        philosopher.join();
    }
}