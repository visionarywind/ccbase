set(HEADER_FILES
        include/tools/tool.h
        include/tools/timer.h
        include/tools/mem.h
        include/tools/set.h)

set(SOURCE_FILES
        src/tool.cpp
        src/mem.cpp
        src/set.cpp)

add_library(tools STATIC ${SOURCE_FILES})

target_include_directories(tools PRIVATE include/tools)
target_include_directories(tools PUBLIC include)
