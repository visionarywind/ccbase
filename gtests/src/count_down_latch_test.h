//
// Created by jiangshanfeng on 2023/7/2.
//

#ifndef CCBASE_HELLO_TEST_H
#define CCBASE_HELLO_TEST_H

#include <dlfcn.h>
#include <iostream>
#include "dlopen_macro.h"
namespace ct {

#define ADD add
#define ADD_FUNC(func, handle) DlsymFuncObj(func, handle)
#define FUNC(handle) ADD_FUNC((ADD), handle)
#define CONCAT(l, r) l##r
#define FUNC_OBJ(func_name) CONCAT(func_name, FunObj)
#define FUNC_DEFINE(func_name, ...) ORIGIN_METHOD(func_name, __VA_ARGS__)

FUNC_DEFINE(ADD, int, int, int)

FUNC_OBJ(ADD) load_func(void *handle) {
    return DlsymFuncObj(add, handle);
}

static constexpr const char *kLibtoolName = "libcommon.dylib";

FUNC_OBJ(ADD) add_;

class HelloTest {
 public:
  HelloTest() {}
};
}  // namespace ct
#endif  // CCBASE_HELLO_TEST_H
