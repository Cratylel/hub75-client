cmake_minimum_required(VERSION 3.14)
project(MatrixBindings LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -g -rdynamic -fPIC")

# Optional defines from environment (you can pass these in via cmake -D)
if(TEST_ENV)
    add_compile_definitions(TEST_ENV)
endif()
if(LOG_LEVEL_DEBUG)
    add_compile_definitions(LOG_LEVEL_DEBUG)
endif()
if(TEST_SERVER)
    add_compile_definitions(TEST_SERVER)
endif()

# Find pybind11
find_package(pybind11 REQUIRED)

# Build the Python module "matrix"
pybind11_add_module(matrix lib/bindings.cpp lib/matrix.cpp)
