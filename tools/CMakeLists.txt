set(HEADER_FILES
        include/tools/tool.h
        include/tools/timer.h)

set(SOURCE_FILES
        src/tool.cpp src/mem.cpp
        include/tools/mem.h)

add_library(tools STATIC ${SOURCE_FILES})

target_include_directories(tools PRIVATE include/tools)
target_include_directories(tools PUBLIC include)
