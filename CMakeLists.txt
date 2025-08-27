cmake_minimum_required(VERSION 3.16)

# 2D simulations (C++20 + raylib)
project(sim2d_visual LANGUAGES CXX)

# Use modern C++ settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Raylib
find_package(raylib CONFIG REQUIRED)

# Main program
add_executable(sim2d src/main.cpp)

# Include headers under "include/"
target_include_directories(sim2d PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link with raylib
target_link_libraries(sim2d PRIVATE raylib)
