#include "ConsumerNode.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <iostream>

ConsumerNode::ConsumerNode(const char* shared_memory_name,
                           uint32_t expected_type)
    : shared_memory_name_(shared_memory_name), expected_type_(expected_type) {}

ConsumerNode::~ConsumerNode() {
    if (header_ != nullptr) {
        header_->consumer_count.fetch_sub(1, std::memory_order_acq_rel);
    }

    CloseSharedMemory();
}

bool ConsumerNode::Initialize() {
    shared_memory_ = OpenSharedMemory(shared_memory_name_, kSharedMemorySize);
    if (shared_memory_.file_descriptor == -1) {
        return false;
    }

    header_ = static_cast<QueueHeader*>(shared_memory_.pointer);

    if (!CheckProtocol()) {
        return false;
    }

    header_->consumer_count.fetch_add(1, std::memory_order_acq_rel);

    return true;
}

bool ConsumerNode::ReadNextMessage(ReceivedMessage& message) {
    while (true) {
        uint64_t head = header_->head.load(std::memory_order_acquire);
        uint64_t publish_tail =
            header_->publish_tail.load(std::memory_order_acquire);
        uint32_t capacity = header_->capacity;

        if (head == publish_tail) {
            return false;
        }

        char* buffer = GetBuffer();

        uint64_t head_index = head % capacity;
        uint64_t bytes_to_end = capacity - head_index;

        if (bytes_to_end < sizeof(MessageHeader)) {
            header_->head.store(head + bytes_to_end, std::memory_order_release);
            continue;
        }

        auto* message_header =
            reinterpret_cast<MessageHeader*>(buffer + head_index);

        if (message_header->type == kPaddingMessageType) {
            uint64_t skip = sizeof(MessageHeader) + message_header->size;
            header_->head.store(head + skip, std::memory_order_release);
            continue;
        }

        uint64_t message_size = sizeof(MessageHeader) + message_header->size;

        if (message_size > capacity || message_size > bytes_to_end) {
            return false;
        }

        if (message_header->type == expected_type_) {
            message.type = message_header->type;
            message.size = message_header->size;
            message.data = buffer + head_index + sizeof(MessageHeader);

            header_->head.store(head + message_size, std::memory_order_release);
            return true;
        } else {
            header_->head.store(head + message_size, std::memory_order_release);
            continue;
        }
    }
}

uint32_t ConsumerNode::GetProducerCount() const {
    return header_->producer_count.load();
}

SharedMemory ConsumerNode::OpenSharedMemory(const char* name, size_t size) {
    int file_descriptor = shm_open(name, O_RDWR, 0666);
    if (file_descriptor == -1) {
        return {-1, nullptr};
    }

    void* pointer = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                         file_descriptor, 0);
    if (pointer == MAP_FAILED) {
        close(file_descriptor);
        return {-1, nullptr};
    }

    return {file_descriptor, pointer};
}

void ConsumerNode::CloseSharedMemory() {
    if (shared_memory_.pointer != nullptr) {
        munmap(shared_memory_.pointer, kSharedMemorySize);
        shared_memory_.pointer = nullptr;
    }

    if (shared_memory_.file_descriptor != -1) {
        close(shared_memory_.file_descriptor);
        shared_memory_.file_descriptor = -1;
    }
}

bool ConsumerNode::CheckProtocol() const {
    return header_->version == kProtocolVersion;
}

char* ConsumerNode::GetBuffer() const {
    return reinterpret_cast<char*>(header_ + 1);
}
