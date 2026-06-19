cmake_minimum_required(VERSION 3.10.0)
project(chip8 VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(ENABLE_SSH2 OFF CACHE BOOL "" FORCE)
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG v1.91.8-docking
)

FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
  imgui-sfml
  GIT_REPOSITORY https://github.com/eliasdaler/imgui-sfml
)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)

FetchContent_MakeAvailable(imgui-sfml)

add_executable(chip8 src/main.cpp src/chip8/chip8.cpp src/chip8/window.cpp src/chip8/ui.cpp)
target_link_libraries(chip8 PRIVATE ImGui-SFML::ImGui-SFML sfml-audio)
