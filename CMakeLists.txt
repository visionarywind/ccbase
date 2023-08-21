cmake_minimum_required(VERSION 3.17)
project(ccbase)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

# enable debug
set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "" FORCE)

add_subdirectory(tools)
add_subdirectory(common)
add_subdirectory(application)
add_subdirectory(libs)
add_subdirectory(gtests)
add_subdirectory(test)

# Configure for google test.
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://gitee.com/mirrors/googletest.git
        GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
