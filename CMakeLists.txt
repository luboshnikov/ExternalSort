cmake_minimum_required(VERSION 3.15)
project(ExternalSort)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(external_sorting main.cpp)
