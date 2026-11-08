
cmake_minimum_required(VERSION 3.20)

project(MovieMatch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# All project source and header files
add_executable(MovieMatch
        src/main.cpp
        src/MovieWindow.cpp
        src/MovieCatalog.cpp
        src/BTree.cpp
        src/HashTable.cpp
        src/DataLoader.cpp

        include/MovieWindow.h
        include/MovieCatalog.h
        include/BTree.h
        include/Movie.h
        include/Rating.h
        include/RatingKey.h
        include/HashTable.h
        include/DataLoader.h
)


target_include_directories(MovieMatch PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


target_link_libraries(MovieMatch PRIVATE
        Qt6::Widgets
)
