cmake_minimum_required(VERSION 3.10)
project(Examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3 -g -rdynamic -fPIC")

# Path to your shared source
set(LIB_PATH ${CMAKE_SOURCE_DIR}/../../lib)
set(LIB_SRC ${LIB_PATH}/matrix.cpp)
include_directories(${LIB_PATH})

# Find all .cpp files in this directory
file(GLOB EXAMPLE_SRCS "*.cpp")

# Build a binary for each example file
foreach(src_file ${EXAMPLE_SRCS})
    get_filename_component(bin_name ${src_file} NAME_WE)
    add_executable(${bin_name} ${src_file} ${LIB_SRC})
endforeach()


