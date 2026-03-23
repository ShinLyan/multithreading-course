#include "thread_pool.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>

TEST(ThreadPool, SimpleExecution) {
    std::atomic<int> counter = 0;

    {
        ThreadPool thread_pool(4);

        for (int i = 0; i < 100; i++) {
            thread_pool.Submit([&counter] { counter.fetch_add(1); });
        }
    }

    EXPECT_EQ(counter.load(), 100);
}

TEST(ThreadPool, FutureResult) {
    ThreadPool thread_pool(4);

    auto future_first = thread_pool.Submit([] { return 10; });
    auto future_second = thread_pool.Submit([] { return 32; });

    EXPECT_EQ(future_first->Get(), 10);
    EXPECT_EQ(future_second->Get(), 32);
}

TEST(ThreadPool, FutureException) {
    ThreadPool thread_pool(4);

    auto future_with_exception = thread_pool.Submit([] {
        throw std::runtime_error("error");
        return 0;
    });

    EXPECT_THROW(future_with_exception->Get(), std::runtime_error);
}

TEST(ThreadPool, VoidTask) {
    ThreadPool thread_pool(4);

    std::atomic<int> counter = 0;

    auto future_void_task =
        thread_pool.Submit([&counter] { counter.fetch_add(1); });

    future_void_task->Get();

    EXPECT_EQ(counter.load(), 1);
}

TEST(ThreadPool, ManyTasks) {
    ThreadPool thread_pool(4);

    std::vector<std::shared_ptr<Future<int>>> futures;

    for (int i = 0; i < 50; i++) {
        futures.push_back(thread_pool.Submit([i] { return i * i; }));
    }

    for (int i = 0; i < 50; i++) {
        EXPECT_EQ(futures[i]->Get(), i * i);
    }
}

TEST(ThreadPool, WaitAndIsReady) {
    ThreadPool thread_pool(2);

    auto future_delayed_result = thread_pool.Submit([] {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 123;
    });

    EXPECT_FALSE(future_delayed_result->IsReady());

    future_delayed_result->Wait();

    EXPECT_TRUE(future_delayed_result->IsReady());
    EXPECT_EQ(future_delayed_result->Get(), 123);
}

TEST(Future, ValidAndDoubleGet) {
    ThreadPool thread_pool(1);

    auto future_value = thread_pool.Submit([] { return 5; });

    EXPECT_TRUE(future_value->Valid());

    EXPECT_EQ(future_value->Get(), 5);

    EXPECT_FALSE(future_value->Valid());

    EXPECT_THROW(future_value->Get(), std::runtime_error);
}

TEST(ThreadPool, NestedSubmit) {
    ThreadPool thread_pool(4);

    auto outer_future = thread_pool.Submit([&thread_pool] {
        auto inner_future = thread_pool.Submit([] { return 21; });
        return inner_future->Get() * 2;
    });

    EXPECT_EQ(outer_future->Get(), 42);
}

TEST(ThreadPool, DestructorWaitsTasks) {
    std::atomic<int> counter = 0;

    {
        ThreadPool thread_pool(2);

        for (int i = 0; i < 8; i++) {
            thread_pool.Submit([&counter] {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                counter.fetch_add(1);
            });
        }
    }

    EXPECT_EQ(counter.load(), 8);
}

TEST(ThreadPool, SubmitWithArguments) {
    ThreadPool thread_pool(4);

    auto future_sum =
        thread_pool.Submit([](int a, int b) { return a + b; }, 10, 32);

    EXPECT_EQ(future_sum->Get(), 42);
}

TEST(ThreadPool, SubmitWithDifferentTypes) {
    ThreadPool thread_pool(2);

    auto future_string = thread_pool.Submit(
        [](int value, std::string prefix) {
            return prefix + std::to_string(value);
        },
        5, std::string("value="));

    EXPECT_EQ(future_string->Get(), "value=5");
}
