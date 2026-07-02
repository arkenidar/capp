cmake_minimum_required(VERSION 3.10)
project(capp C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

add_executable(capp src/main.c)
target_link_libraries(capp SDL2::SDL2)
