cmake_minimum_required(VERSION 3.10)

project(lab2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основная программа
add_executable(lab2 src/main.cpp src/Threads.cpp)

target_include_directories(lab2 PRIVATE src)

# Включаем тесты
enable_testing()
add_subdirectory(tests)