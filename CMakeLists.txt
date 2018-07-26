# Specify the minimum version for CMake

cmake_minimum_required(VERSION 2.8)

# Project's name

project(lineCounter)
# Set the output folder where your program will be created
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_SOURCE_DIR})

add_subdirectory(src)
add_executable(lineCounter ${PROJECT_SOURCE_DIR}/lineCounter.cpp)