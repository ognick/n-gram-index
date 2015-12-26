cmake_minimum_required(VERSION 2.6)
project(indexer)

set(CMAKE_CXX_FLAGS "-std=c++0x -g")

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(indexer_lib)
add_subdirectory(python_extention)
