#include <tools/tool.h>
#include <catch.hpp>

TEST_CASE("Create and count", "[count_down_latch]") {
    auto count_down_latch = CountDownLatch(10);
    count_down_latch.count_down();
    REQUIRE(count_down_latch.count() == 9);
}