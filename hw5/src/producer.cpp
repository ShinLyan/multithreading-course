#include <sys/mman.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>

#include "ProducerNode.hpp"
#include "Queue.hpp"

std::atomic<bool> running{true};
const char* kSharedMemoryName = "/my_shared_memory";

void SignalHandler(int) {
    std::cout << "\n[producer] shutting down...\n";
    running = false;
}

int main(int argc, char** argv) {
    signal(SIGINT, SignalHandler);

    int producer_id = 1;
    if (argc > 1) {
        producer_id = std::atoi(argv[1]);
    }

    ProducerNode producer(kSharedMemoryName);

    if (!producer.Initialize()) {
        return 1;
    }

    std::cout << "[producer " << producer_id << "] started\n";

    int counter = 0;

    while (running) {
        if (producer_id == 1) {
            std::string text = "hello from producer " +
                               std::to_string(producer_id) + " #" +
                               std::to_string(counter);

            producer.Send(kTextMessageType, text.c_str(), text.size() + 1);

            std::cout << "[producer " << producer_id << "] sent TEXT: " << text
                      << "\n";
        } else {
            int value = counter;

            producer.Send(kNumberMessageType, &value, sizeof(value));

            std::cout << "[producer " << producer_id
                      << "] sent NUMBER: " << value << "\n";
        }

        counter++;

        int delay_ms = 1000 + producer_id * 300;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    }

    if (producer.ShouldCleanup()) {
        shm_unlink(kSharedMemoryName);
        std::cout << "[producer] shared memory removed\n";
    } else {
        std::cout << "[producer] skip cleanup (others still running)\n";
    }

    return 0;
}
