//
// Created by jiangshanfeng on 2023/7/2.
//

#ifndef CCBASE_HELLO_TEST_H
#define CCBASE_HELLO_TEST_H

#include <dlfcn.h>
#include <iostream>
#include "dlopen_macro.h"
namespace ct {
ORIGIN_METHOD(add, int, int, int);

static constexpr const char *kLibtoolName = "libcommon.dylib";

addFunObj add_ = nullptr;

class HelloTest {
 public:
  HelloTest() {}
};
}  // namespace ct
#endif  // CCBASE_HELLO_TEST_H
