project(ccbase)

set(SOURCE_FILES src/main.cpp
        src/inherit.cpp src/ref.cpp src/ref.h ../nice/include/hqueue.h)

add_executable(app ${SOURCE_FILES})

target_link_libraries(app LINK_PRIVATE tools)
