set(SOURCE_FILES
        src/common.cpp
)

add_library(common SHARED ${SOURCE_FILES})

target_include_directories(tools PRIVATE include/common)
target_include_directories(tools PUBLIC include)
