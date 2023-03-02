# This is the same version as project bergamot
cmake_minimum_required(VERSION 3.5.1)

project(CMakeSpec
    VERSION 1.0.0
    DESCRIPTION "An example of how to build using CMake."
    LANGUAGES CXX
)

set(Sources
  src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Specify the build directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
