//
// Created by jiangshanfeng on 2023/7/2.
//
#include <gtest/gtest.h>
#include <iostream>
#include <tools/tool.h>
#include <tools/set.h>
#include "count_down_latch_test.h"

using namespace std;
namespace ct {

TEST(CountDownLatchTest, TestCountDownLatch) {
  cout << "Enter unit test : TestCountDownLatch." << endl;
  auto count_down_latch_ptr = std::make_shared<CountDownLatch>(1);
  count_down_latch_ptr->count_down();
  EXPECT_EQ(0, count_down_latch_ptr->count());
}

TEST(CountDownLatchTest, TestSet) {
  cout << "Enter unit test : TestSet." << endl;
  Set *set_ptr = new Set();
  std::shared_ptr<int> p = nullptr;
  p.get();
  // Test nullptr with offset.
  int *intp = nullptr;
  set_ptr->test();
  void *vp = intp + 100;
  cout << vp << endl;
  set_ptr->container.emplace("1");
  (void)set_ptr->container.emplace("2");
  EXPECT_EQ(2, set_ptr->container.size());
}

TEST(CountDownLatchTest, TestCommon) {
  cout << "Enter unit test : TestCommon." << endl;
  void *handle = dlopen(kLibtoolName, RTLD_NOW);

  auto handle_ptr = std::shared_ptr<void *>(&handle, [](void **ptr) {
        if (ptr != nullptr) {
          cout << "Close GMem lib : " << *ptr << endl;
          if (dlclose(*ptr) != 0) {
            cout << "Close GMem lib failed." << endl;
          }
        }
      });

  std::cout << "handle " << handle << std::endl;
  if (handle != nullptr) {
      #define To(x) x
    add_ = DlsymFuncObj(add, handle);
  }
  if (add_) {
    cout << "add : " << add_(1, 2) << endl;
  } else {
    cout << "add_ is nullptr" << endl;
  }
}
}  // namespace ct