project(algorithm)

file(GLOB_RECURSE SOURCE_FILES "*.cc")

add_library(algorithm ${SOURCE_FILES})
