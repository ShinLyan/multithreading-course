#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>

template <class T>
class BufferedChannel {
   public:
    explicit BufferedChannel(int size) : capacity_(size) {
        if (size <= 0) {
            throw std::invalid_argument("size must be > 0");
        }
    }

    void Send(const T& value) {
        std::unique_lock<std::mutex> lock(mutex_);

        can_send_.wait(lock,
                       [&] { return closed_ || queue_.size() < capacity_; });

        if (closed_) {
            throw std::runtime_error("Channel is closed");
        }

        queue_.push(value);

        can_recv_.notify_one();
    }

    std::optional<T> Recv() {
        std::unique_lock<std::mutex> lock(mutex_);

        can_recv_.wait(lock, [&] { return closed_ || !queue_.empty(); });

        if (queue_.empty()) return std::nullopt;

        T value = std::move(queue_.front());
        queue_.pop();

        can_send_.notify_one();

        return value;
    }

    void Close() {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
        can_send_.notify_all();
        can_recv_.notify_all();
    }

   private:
    std::mutex mutex_;
    std::condition_variable can_send_;
    std::condition_variable can_recv_;
    std::queue<T> queue_;
    int capacity_;
    bool closed_ = false;
};
