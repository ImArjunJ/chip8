cmake_minimum_required(VERSION 3.10.0)
project(chip8 VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(chip8 src/main.cpp)

