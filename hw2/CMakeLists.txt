cmake_minimum_required(VERSION 3.16)
project(hw2 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest benchmark)

enable_testing()

add_executable(apply_function_tests
    tests/apply_function_tests.cpp
)

target_include_directories(apply_function_tests PRIVATE include)

target_link_libraries(apply_function_tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(apply_function_tests)

add_executable(apply_function_benchmark
    benchmarks/apply_function_benchmark.cpp
)

target_include_directories(apply_function_benchmark PRIVATE include)

target_link_libraries(apply_function_benchmark
    benchmark::benchmark
)