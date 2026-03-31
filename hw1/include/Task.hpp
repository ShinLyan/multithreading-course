#pragma once

#include <coroutine>
#include <cstdint>
#include <exception>

class Task {
   public:
    struct promise_type {
        uint32_t value_{};

        Task get_return_object() {
            return Task{
                std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() { return {}; }

        std::suspend_always final_suspend() noexcept { return {}; }

        void unhandled_exception() {
            std::rethrow_exception(std::current_exception());
        }

        std::suspend_always yield_value(uint32_t value) {
            value_ = value;
            return {};
        }

        void return_void() {}
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Task(Handle handle) : handle_(handle) {}

    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    uint32_t Next() {
        handle_.resume();
        return handle_.promise().value_;
    }

    bool Done() const { return handle_.done(); }

    Handle handle_;
};
