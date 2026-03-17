#include <benchmark/benchmark.h>

#include <cmath>
#include <vector>

#include "apply_function.h"

namespace {

std::vector<int> MakeVector(const std::size_t size) {
    std::vector<int> data(size);
    for (std::size_t i = 0; i < size; i++) {
        data[i] = static_cast<int>(i % 1000);
    }
    return data;
}

void LightTransform(int& x) {
    x += 1;
}

void HeavyTransform(int& x) {
    double value = static_cast<double>(x);

    for (int i = 0; i < 1000; i++) {
        value = std::sin(value) * std::cos(value) + std::sqrt(value * value + 1.0);
    }

    x = static_cast<int>(value);
}

static void BM_SingleThread_LightSmall(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> data = MakeVector(1000);
        ApplyFunction<int>(data, LightTransform, 1);
        benchmark::DoNotOptimize(data);
    }
}
BENCHMARK(BM_SingleThread_LightSmall);

static void BM_MultiThread_LightSmall(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> data = MakeVector(1000);
        ApplyFunction<int>(data, LightTransform, 4);
        benchmark::DoNotOptimize(data);
    }
}
BENCHMARK(BM_MultiThread_LightSmall);

static void BM_SingleThread_HeavyLarge(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> data = MakeVector(200000);
        ApplyFunction<int>(data, HeavyTransform, 1);
        benchmark::DoNotOptimize(data);
    }
}
BENCHMARK(BM_SingleThread_HeavyLarge);

static void BM_MultiThread_HeavyLarge(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> data = MakeVector(200000);
        ApplyFunction<int>(data, HeavyTransform, 4);
        benchmark::DoNotOptimize(data);
    }
}
BENCHMARK(BM_MultiThread_HeavyLarge);

}

BENCHMARK_MAIN();