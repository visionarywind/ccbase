set(SOURCE_FILES
        src/tool.cpp
        src/mem.cpp
        src/set.cpp)

add_library(tools SHARED ${SOURCE_FILES})

target_include_directories(tools PRIVATE include/tools)
target_include_directories(tools PUBLIC include)
