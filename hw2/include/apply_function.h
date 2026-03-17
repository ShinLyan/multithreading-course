#pragma once

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <thread>
#include <vector>

template <typename T>
void ApplyFunction(std::vector<T>& data,
                   const std::function<void(T&)>& transform,
                   const int threadCount = 1) {
    if (threadCount <= 0) {
        throw std::invalid_argument("threadCount must be positive");
    }

    if (data.empty()) {
        return;
    }

    const int actualThreadCount =
        std::min<int>(threadCount, static_cast<int>(data.size()));

    if (actualThreadCount == 1) {
        for (T& item : data) {
            transform(item);
        }
        return;
    }

    std::vector<std::thread> threads;
    threads.reserve(actualThreadCount);

    const std::size_t totalSize = data.size();
    const std::size_t baseChunkSize = totalSize / actualThreadCount;
    const std::size_t remainder = totalSize % actualThreadCount;

    std::size_t begin = 0;

    for (int i = 0; i < actualThreadCount; i++) {
        const std::size_t currentChunkSize =
            baseChunkSize + (static_cast<std::size_t>(i) < remainder ? 1 : 0);
        const std::size_t end = begin + currentChunkSize;

        threads.emplace_back([begin, end, &data, &transform]() {
            for (std::size_t j = begin; j < end; j++) {
                transform(data[j]);
            }
        });

        begin = end;
    }

    for (std::thread& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}