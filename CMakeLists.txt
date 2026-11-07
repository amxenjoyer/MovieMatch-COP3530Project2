cmake_minimum_required(VERSION 3.20)

project(MovieMatch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MovieMatch
        src/main.cpp
)

target_include_directories(MovieMatch PRIVATE include)