#pragma once

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>

void FutexWait(void* value, int expectedValue) {
    syscall(SYS_futex, value, FUTEX_WAIT_PRIVATE, expectedValue, nullptr,
            nullptr, 0);
}

void FutexWake(void* value, int count) {
    syscall(SYS_futex, value, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr, 0);
}

class Mutex {
   public:
    Mutex() = default;

    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    void Lock() {
        int expected = 0;
        if (state_.compare_exchange_strong(expected, 1,
                                           std::memory_order_acquire,
                                           std::memory_order_relaxed)) {
            return;
        }

        while (true) {
            expected = 0;
            if (state_.compare_exchange_strong(expected, 2,
                                               std::memory_order_acquire,
                                               std::memory_order_relaxed)) {
                return;
            }

            if (expected == 1) {
                if (state_.compare_exchange_strong(expected, 2,
                                                   std::memory_order_acquire,
                                                   std::memory_order_relaxed)) {
                    FutexWait(&state_, 2);
                } else {
                    continue;
                }
            } else {
                FutexWait(&state_, 2);
            }
        }
    }

    void Unlock() {
        const int old = state_.fetch_sub(1, std::memory_order_release);

        if (old != 1) {
            state_.store(0, std::memory_order_release);
            FutexWake(&state_, 1);
        }
    }

   private:
    std::atomic<int> state_{0};
};
