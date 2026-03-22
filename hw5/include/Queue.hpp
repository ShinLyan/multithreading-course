#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

// --- protocol constants ---
inline constexpr uint32_t kProtocolVersion = 1;
inline constexpr uint32_t kPaddingMessageType = 0;
inline constexpr uint32_t kTextMessageType = 1;
inline constexpr uint32_t kNumberMessageType = 2;

// --- shared memory config ---
inline constexpr size_t kSharedMemorySize = 4096;
inline constexpr uint32_t kQueueCapacity = 1024;

// --- queue layout ---
struct QueueHeader {
    uint32_t version;
    uint32_t capacity;

    std::atomic<uint64_t> head;
    std::atomic<uint64_t> reserve_tail;
    std::atomic<uint64_t> publish_tail;

    std::atomic<uint32_t> producer_count;
    std::atomic<uint32_t> consumer_count;
};

// --- message format ---
struct MessageHeader {
    uint32_t type;
    uint32_t size;
};

// --- shared memory helper ---
struct SharedMemory {
    int file_descriptor;
    void* pointer;
};
