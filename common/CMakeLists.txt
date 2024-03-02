project(common)

include_directories(include)

set(SOURCE_FILES
        src/common.cpp
)

add_library(common SHARED OBJECT ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
        INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        )