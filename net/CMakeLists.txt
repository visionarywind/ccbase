set(SOURCE_FILES src/*.cc)

add_library(net SHARED ${SOURCE_FILES})

target_include_directories(net PUBLIC include)
