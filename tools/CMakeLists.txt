project(tools)

include_directories(include)

set(SOURCE_FILES
        src/tool.cpp
        src/mem.cpp
        src/set.cpp)

add_library(tools SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
        INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        )
