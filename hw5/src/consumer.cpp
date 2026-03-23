#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

#include "ConsumerNode.hpp"
#include "Queue.hpp"

int main(int argc, char** argv) {
    uint32_t expected_type = kTextMessageType;

    if (argc > 1) {
        expected_type = static_cast<uint32_t>(std::atoi(argv[1]));
    }

    ConsumerNode consumer("/my_shared_memory", expected_type);

    if (!consumer.Initialize()) {
        return 1;
    }

    std::cout << "[consumer] started, type=" << expected_type << "\n";

    while (true) {
        ReceivedMessage msg;

        bool got = consumer.ReadNextMessage(msg);

        if (got) {
            std::cout << "[consumer] message\n";
            std::cout << "  type: " << msg.type << "\n";
            std::cout << "  size: " << msg.size << "\n";

            if (msg.type == kNumberMessageType && msg.size == sizeof(int)) {
                int value;
                std::memcpy(&value, msg.data.data(), sizeof(int));

                std::cout << "  data (int): " << value << "\n";
            } else {
                std::cout << "  data (text): " << msg.data.data() << "\n";
            }
        } else {
            if (consumer.GetProducerCount() == 0) {
                std::cout << "[consumer] no producers left, exiting\n";
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
}
