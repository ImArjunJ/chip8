cmake_minimum_required(VERSION 3.10.0)
project(chip8 VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(chip8 src/main.cpp src/chip8/chip8.cpp)
target_link_libraries(chip8 PRIVATE sfml-graphics sfml-audio)


