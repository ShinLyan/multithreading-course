#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "mutex.h"

void RunStdMutexTest() {
    std::mutex mutex;
    int counter = 0;

    static constexpr int kThreads = 4;
    static constexpr int kIters = 100000;

    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int i = 0; i < kThreads; i++) {
        threads.emplace_back([&]() {
            for (int j = 0; j < kIters; j++) {
                mutex.lock();
                counter++;
                mutex.unlock();
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "std::mutex:\n";
    std::cout << "counter = " << counter << '\n';
    std::cout << "expected = " << kThreads * kIters << "\n\n";
}

void RunMyMutexTest() {
    Mutex mutex;
    int counter = 0;

    static constexpr int kThreads = 4;
    static constexpr int kIters = 100000;

    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int i = 0; i < kThreads; i++) {
        threads.emplace_back([&]() {
            for (int j = 0; j < kIters; j++) {
                mutex.Lock();
                counter++;
                mutex.Unlock();
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "My Mutex:\n";
    std::cout << "counter = " << counter << '\n';
    std::cout << "expected = " << kThreads * kIters << "\n\n";
}

int main() {
    RunStdMutexTest();
    RunMyMutexTest();

    return 0;
}
