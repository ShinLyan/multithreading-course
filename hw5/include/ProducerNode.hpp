#pragma once

#include "Queue.hpp"

class ProducerNode {
   public:
    explicit ProducerNode(const char* shared_memory_name);
    ~ProducerNode();

    bool Initialize();
    bool Send(uint32_t type, const void* data, uint32_t size);

    bool ShouldCleanup() const;

   private:
    SharedMemory CreateSharedMemory(const char* name, size_t size);
    void CloseSharedMemory();
    void InitializeQueue();
    char* GetBuffer() const;

    bool TryReserveSpace(uint64_t message_size, uint64_t& reserved_begin,
                         uint64_t& reserved_end, bool& needs_wrap_padding);

    void WritePadding(uint64_t reserved_begin);
    void WriteMessage(uint64_t reserved_begin, const char* text,
                      uint32_t text_size);
    void PublishReservation(uint64_t reserved_begin, uint64_t reserved_end);

   private:
    const char* shared_memory_name_;
    SharedMemory shared_memory_{-1, nullptr};
    QueueHeader* header_ = nullptr;
};
