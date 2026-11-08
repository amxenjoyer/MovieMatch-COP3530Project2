
cmake_minimum_required(VERSION 3.16)

project(MovieMatch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Location of Qt on your computer
set(CMAKE_PREFIX_PATH "C:/Qt/6.12.0/mingw_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(MovieMatch
        main.cpp
)

target_link_libraries(MovieMatch PRIVATE Qt6::Widgets)
