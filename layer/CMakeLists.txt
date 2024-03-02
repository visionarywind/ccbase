project(layer)


include_directories(include)

set(SOURCE_FILES
        src/layer.cpp
)

add_library(layer SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME}
        INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        )


# set_property(TARGET layer PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(layer LINK_PRIVATE common)