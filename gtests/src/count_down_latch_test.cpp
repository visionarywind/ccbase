//
// Created by jiangshanfeng on 2023/7/2.
//
#include <gtest/gtest.h>
#include <iostream>
#include "tools/tool.h"
#include "tools/set.h"

using namespace std;

TEST(HelloTest, TestCountDownLatch) {
    cout << "CountDownLatch unit test." << endl;
    auto count_down_latch_ptr = std::make_shared<CountDownLatch>(1);
    count_down_latch_ptr->count_down();
    EXPECT_EQ(0, count_down_latch_ptr->count());
}

TEST(HelloTest, TestSet) {
    cout << "Set unit test." << endl;
    Set *set_ptr = new Set();
    set_ptr->test();
    set_ptr->container.emplace("1");
    (void)set_ptr->container.emplace("2");
    EXPECT_EQ(2, set_ptr->container.size());
}