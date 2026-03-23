#pragma once

#include <condition_variable>
#include <exception>
#include <mutex>
#include <stdexcept>

template <typename T>
class Future {
   public:
    Future() = default;

    void SetValue(const T& value) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            value_ = value;
            is_ready_ = true;
        }
        condition_variable_.notify_all();
    }

    void SetException(std::exception_ptr exception) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            exception_ = exception;
            is_ready_ = true;
        }
        condition_variable_.notify_all();
    }

    void Wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_variable_.wait(lock, [this] { return is_ready_; });
    }

    bool IsReady() {
        std::unique_lock<std::mutex> lock(mutex_);
        return is_ready_;
    }

    bool Valid() {
        std::unique_lock<std::mutex> lock(mutex_);
        return is_valid_;
    }

    T Get() {
        std::unique_lock<std::mutex> lock(mutex_);

        if (!is_valid_) {
            throw std::runtime_error("Future is not valid");
        }

        condition_variable_.wait(lock, [this] { return is_ready_; });

        is_valid_ = false;

        if (exception_) {
            std::rethrow_exception(exception_);
        }

        return value_;
    }

   private:
    T value_;
    bool is_ready_ = false;
    bool is_valid_ = true;

    std::exception_ptr exception_;

    std::mutex mutex_;
    std::condition_variable condition_variable_;
};

template <>
class Future<void> {
   public:
    Future() = default;

    void SetValue() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            is_ready_ = true;
        }
        condition_variable_.notify_all();
    }

    void SetException(std::exception_ptr exception) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            exception_ = exception;
            is_ready_ = true;
        }
        condition_variable_.notify_all();
    }

    void Wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_variable_.wait(lock, [this] { return is_ready_; });
    }

    bool IsReady() {
        std::unique_lock<std::mutex> lock(mutex_);
        return is_ready_;
    }

    bool Valid() {
        std::unique_lock<std::mutex> lock(mutex_);
        return is_valid_;
    }

    void Get() {
        std::unique_lock<std::mutex> lock(mutex_);

        if (!is_valid_) {
            throw std::runtime_error("Future is not valid");
        }

        condition_variable_.wait(lock, [this] { return is_ready_; });

        is_valid_ = false;

        if (exception_) {
            std::rethrow_exception(exception_);
        }
    }

   private:
    bool is_ready_ = false;
    bool is_valid_ = true;

    std::exception_ptr exception_;

    std::mutex mutex_;
    std::condition_variable condition_variable_;
};
