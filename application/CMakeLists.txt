set(SOURCE_FILES src/main.cpp
        src/inherit.cpp)

add_executable(app ${SOURCE_FILES})

target_link_libraries(app LINK_PRIVATE tools)
