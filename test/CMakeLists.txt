set(TEST_SOURCES
        src/tool_test.cpp
        src/mem_test.cpp)

enable_testing()

add_executable(run_tests
        ${TEST_SOURCES}
        src/run_tests.cpp)

target_link_libraries(run_tests LINK_PRIVATE catch2 tools)

