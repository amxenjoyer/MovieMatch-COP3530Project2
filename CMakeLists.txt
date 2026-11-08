
cmake_minimum_required(VERSION 3.20)

project(MovieMatch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 6 is required for the graphical interface
find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)

add_executable(MovieMatch
        main.cpp

        MovieWindow.h
        MovieWindow.cpp

        MovieCatalog.h
        MovieCatalog.cpp

        BTree.h
        BTree.cpp

        include/Movie.h
        include/Rating.h
        include/RatingKey.h
        include/HashTable.h
        include/DataLoader.h

        src/HashTable.cpp
        src/DataLoader.cpp
        MovieCatalog.h
        MovieCatalog.h
        MovieCatalog.h
        MovieCatalog.cpp
        MovieWindow.h
        MovieWindow.cpp
)

target_include_directories(MovieMatch PRIVATE
        include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MovieMatch PRIVATE
        Qt6::Widgets
        Qt6::Concurrent
)
