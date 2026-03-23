#include "ProducerNode.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <atomic>
#include <cstring>

ProducerNode::ProducerNode(const char* shared_memory_name)
    : shared_memory_name_(shared_memory_name) {}

ProducerNode::~ProducerNode() {
    if (header_ != nullptr) {
        header_->producer_count.fetch_sub(1, std::memory_order_acq_rel);
    }

    CloseSharedMemory();
}

bool ProducerNode::Initialize() {
    shared_memory_ = CreateSharedMemory(shared_memory_name_, kSharedMemorySize);
    if (shared_memory_.file_descriptor == -1) {
        return false;
    }

    header_ = static_cast<QueueHeader*>(shared_memory_.pointer);

    InitializeQueue();

    header_->producer_count.fetch_add(1, std::memory_order_acq_rel);

    return true;
}

bool ProducerNode::Send(uint32_t type, const void* data, uint32_t size) {
    uint64_t message_size = sizeof(MessageHeader) + size;

    if (message_size > header_->capacity) {
        return false;
    }

    uint64_t reserved_begin = 0;
    uint64_t reserved_end = 0;
    bool needs_wrap_padding = false;

    if (!TryReserveSpace(message_size, reserved_begin, reserved_end,
                         needs_wrap_padding)) {
        return false;
    }

    if (needs_wrap_padding) {
        WritePadding(reserved_begin);
    }

    uint64_t capacity = header_->capacity;
    uint64_t tail_index = reserved_begin % capacity;
    uint64_t bytes_to_end = capacity - tail_index;

    if (sizeof(MessageHeader) + size > bytes_to_end) {
        tail_index = 0;
    }

    MessageHeader message_header{type, size};

    char* buffer = GetBuffer();
    std::memcpy(buffer + tail_index, &message_header, sizeof(message_header));
    std::memcpy(buffer + tail_index + sizeof(message_header), data, size);

    PublishReservation(reserved_begin, reserved_end);
    return true;
}

bool ProducerNode::ShouldCleanup() const {
    uint32_t producers = header_->producer_count.load();
    return producers == 1;
}

SharedMemory ProducerNode::CreateSharedMemory(const char* name, size_t size) {
    int file_descriptor = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (file_descriptor == -1) {
        return {-1, nullptr};
    }

    if (ftruncate(file_descriptor, size) == -1) {
        close(file_descriptor);
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

void ProducerNode::CloseSharedMemory() {
    if (shared_memory_.pointer != nullptr) {
        munmap(shared_memory_.pointer, kSharedMemorySize);
        shared_memory_.pointer = nullptr;
    }

    if (shared_memory_.file_descriptor != -1) {
        close(shared_memory_.file_descriptor);
        shared_memory_.file_descriptor = -1;
    }
}

void ProducerNode::InitializeQueue() {
    if (header_->version != kProtocolVersion) {
        header_->version = kProtocolVersion;
        header_->capacity = kQueueCapacity;

        header_->head.store(0, std::memory_order_relaxed);
        header_->reserve_tail.store(0, std::memory_order_relaxed);
        header_->publish_tail.store(0, std::memory_order_relaxed);

        header_->producer_count.store(0, std::memory_order_relaxed);
        header_->consumer_count.store(0, std::memory_order_relaxed);
    }
}

char* ProducerNode::GetBuffer() const {
    return reinterpret_cast<char*>(header_ + 1);
}

bool ProducerNode::TryReserveSpace(uint64_t message_size,
                                   uint64_t& reserved_begin,
                                   uint64_t& reserved_end,
                                   bool& needs_wrap_padding) {
    uint64_t capacity = header_->capacity;

    while (true) {
        uint64_t head = header_->head.load(std::memory_order_acquire);
        uint64_t current_reserve_tail =
            header_->reserve_tail.load(std::memory_order_acquire);

        uint64_t used_space = current_reserve_tail - head;
        if (used_space > capacity) {
            return false;
        }

        uint64_t free_space = capacity - used_space;
        uint64_t tail_index = current_reserve_tail % capacity;
        uint64_t bytes_to_end = capacity - tail_index;

        uint64_t required_space = message_size;
        bool wrap_padding_required = false;

        if (message_size > bytes_to_end) {
            required_space += bytes_to_end;
            wrap_padding_required = true;
        }

        if (required_space > free_space) {
            return false;
        }

        uint64_t next_reserve_tail = current_reserve_tail + required_space;

        if (header_->reserve_tail.compare_exchange_weak(
                current_reserve_tail, next_reserve_tail,
                std::memory_order_acq_rel, std::memory_order_acquire)) {
            reserved_begin = current_reserve_tail;
            reserved_end = next_reserve_tail;
            needs_wrap_padding = wrap_padding_required;
            return true;
        }
    }
}

void ProducerNode::WritePadding(uint64_t reserved_begin) {
    uint64_t capacity = header_->capacity;
    uint64_t tail_index = reserved_begin % capacity;
    uint64_t bytes_to_end = capacity - tail_index;

    if (bytes_to_end >= sizeof(MessageHeader)) {
        MessageHeader padding_header{
            kPaddingMessageType,
            static_cast<uint32_t>(bytes_to_end - sizeof(MessageHeader))};

        char* buffer = GetBuffer();
        std::memcpy(buffer + tail_index, &padding_header,
                    sizeof(padding_header));
    }
}

void ProducerNode::WriteMessage(uint64_t reserved_begin, const char* text,
                                uint32_t text_size) {
    uint64_t capacity = header_->capacity;
    uint64_t tail_index = reserved_begin % capacity;
    uint64_t bytes_to_end = capacity - tail_index;

    if (sizeof(MessageHeader) + text_size > bytes_to_end) {
        tail_index = 0;
    }

    MessageHeader message_header{kTextMessageType, text_size};

    char* buffer = GetBuffer();
    std::memcpy(buffer + tail_index, &message_header, sizeof(message_header));
    std::memcpy(buffer + tail_index + sizeof(message_header), text, text_size);
}

void ProducerNode::PublishReservation(uint64_t reserved_begin,
                                      uint64_t reserved_end) {
    while (header_->publish_tail.load(std::memory_order_acquire) !=
           reserved_begin) {
    }

    header_->publish_tail.store(reserved_end, std::memory_order_release);
}
