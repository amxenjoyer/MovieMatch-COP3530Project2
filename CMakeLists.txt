cmake_minimum_required(VERSION 3.16)
project(MovieMatch LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(BTreeTest
        BTreeTest.cpp
        BTree.cpp
        BTree.h
)