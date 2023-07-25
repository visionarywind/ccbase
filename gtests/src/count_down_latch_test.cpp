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
    Set set;
    set.test();
}