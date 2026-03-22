#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "future.h"

class ThreadPool {
   public:
    explicit ThreadPool(size_t thread_count) {
        for (size_t i = 0; i < thread_count; i++) {
            worker_threads_.emplace_back([this] { WorkerLoop(); });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            is_stopping_ = true;
        }

        condition_variable_.notify_all();

        for (auto& worker_thread : worker_threads_) {
            worker_thread.join();
        }
    }

    template <typename Func, typename... Args>
    auto Submit(Func&& func, Args&&... args) {
        using ResultType = std::invoke_result_t<Func, Args...>;

        auto future = std::make_shared<Future<ResultType>>();

        auto task_wrapper = [func = std::forward<Func>(func),
                             args_tuple =
                                 std::make_tuple(std::forward<Args>(args)...),
                             future]() mutable {
            try {
                if constexpr (std::is_void_v<ResultType>) {
                    std::apply(func, args_tuple);
                    future->SetValue();
                } else {
                    ResultType result = std::apply(func, args_tuple);
                    future->SetValue(result);
                }
            } catch (...) {
                future->SetException(std::current_exception());
            }
        };

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);

            if (is_stopping_) {
                throw std::runtime_error("ThreadPool is stopping");
            }

            task_queue_.push(std::move(task_wrapper));
        }

        condition_variable_.notify_one();

        return future;
    }

   private:
    void WorkerLoop() {
        while (true) {
            std::function<void()> current_task;

            {
                std::unique_lock<std::mutex> lock(queue_mutex_);

                condition_variable_.wait(lock, [this] {
                    return is_stopping_ || !task_queue_.empty();
                });

                if (is_stopping_ && task_queue_.empty()) {
                    return;
                }

                current_task = std::move(task_queue_.front());
                task_queue_.pop();
            }

            current_task();
        }
    }

   private:
    std::vector<std::thread> worker_threads_;
    std::queue<std::function<void()>> task_queue_;

    std::mutex queue_mutex_;
    std::condition_variable condition_variable_;
    bool is_stopping_ = false;
};
