cmake_minimum_required(VERSION 3.17)
project(ccbase)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

add_subdirectory(tools)
add_subdirectory(application)
add_subdirectory(libs)
add_subdirectory(test)
