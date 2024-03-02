//
// Created by jiangshanfeng on 2023/6/25.
//

#include <iostream>
#include <atomic>
#include "common/common.h"

int add(int left, int right) {
  static std::atomic<int> kCache = 1;
  return left + right + kCache.fetch_add(1);
}