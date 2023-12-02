//
// Created by jiangshanfeng on 2023/6/25.
//

#pragma once

#include <string>
#include <iostream>

extern "C" int add(int left, int right);

inline bool endsWith(std::string left, std::string right) {
  return left.rfind(right) == left.size() - right.size();
}