cmake_minimum_required(VERSION 3.16)

project(futex_mutex LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(futex_mutex main.cpp)

target_link_libraries(futex_mutex PRIVATE Threads::Threads)