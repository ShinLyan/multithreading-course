cmake_minimum_required(VERSION 3.16)

project(cooperative_dfs CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(include)

add_executable(cooperative_dfs
    src/main.cpp
)
