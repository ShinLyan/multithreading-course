#pragma once

#include "Queue.hpp"

struct ReceivedMessage {
    uint32_t type;
    uint32_t size;
    const char* data;
};

class ConsumerNode {
   public:
    ConsumerNode(const char* shared_memory_name, uint32_t expected_type);
    ~ConsumerNode();

    bool Initialize();
    bool ReadNextMessage(ReceivedMessage& message);

    uint32_t GetProducerCount() const;

   private:
    SharedMemory OpenSharedMemory(const char* name, size_t size);
    void CloseSharedMemory();
    bool CheckProtocol() const;
    char* GetBuffer() const;

   private:
    const char* shared_memory_name_;
    uint32_t expected_type_;

    SharedMemory shared_memory_{-1, nullptr};
    QueueHeader* header_ = nullptr;
};
