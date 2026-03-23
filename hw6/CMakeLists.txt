cmake_minimum_required(VERSION 3.16)

project(thread_pool CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(include)

find_package(GTest REQUIRED)

add_executable(thread_pool_tests
    tests/thread_pool_test.cpp
)

target_link_libraries(thread_pool_tests
    GTest::GTest
    GTest::Main
    pthread
)
