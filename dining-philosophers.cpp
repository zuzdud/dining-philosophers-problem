#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

// vector for storing philosophers threads
std::vector<std::thread> philosophersThreads;

// func that will be run by each thread
void runPhilosLife(const int philosophersId){
    std::cout << "Philosopher " << philosophersId << " started thinking..." << std::endl;
}

int main(){
    int numberOfPhilosophers;
    std::cout << "Dining Philosophers Problem lets gooo" << std::endl << std::endl;
    std::cout << "How many philosophers are at the table?" << std::endl;
    std::cin >> numberOfPhilosophers;

    // create threads
    for (int i = 0; i < numberOfPhilosophers; i++){
        philosophersThreads.emplace_back(runPhilosLife, i);
    }
    
    // wait for them to end
    for(auto& philosopher : philosophersThreads){
        philosopher.join();
    }
    
}