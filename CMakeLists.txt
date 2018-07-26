# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.5)

# Project's name
project(lineCounter)
set (CMAKE_CXX_STANDARD 14)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Set the output folder where your program will be created
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_HOME_DIRECTORY}/linux)

add_subdirectory(src)


