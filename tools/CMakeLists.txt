project(tools)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_FILES
        src/tool.cpp
        src/mem.cpp
        src/set.cpp)

add_library(tools SHARED ${SOURCE_FILES})

target_include_directories(tools INTERFACE .)
