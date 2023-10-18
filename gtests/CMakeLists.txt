project(gtests)

set(TEST_SOURCES
        src/count_down_latch_test.cpp
        src/count_down_latch_test.h)

enable_testing()

add_executable(
        gtests
        src/count_down_latch_test.cpp
)

target_link_libraries(
        gtests
        GTest::gtest_main
        tools
        common
)

include(GoogleTest)
gtest_discover_tests(gtests)
