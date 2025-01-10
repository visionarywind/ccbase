#include <iostream>
// #include <dlfcn.h>
#include "../lib/generator_lib.h"
#include "lib_loader1.h"

#include "../lib2/lib_loader2.h"
using namespace std;

void load1() {
//   void *handle = dlopen("./generator.so", RTLD_LAZY);
//   using GenerateFunc = int (*)();
//   GenerateFunc generator_func = (GenerateFunc)dlsym(handle, "generate");
//   const char *dlsym_error = dlerror();
//   if (dlsym_error) {
//     std::cerr << "无法获取函数地址: " << dlsym_error << std::endl;
//     dlclose(handle);
//     return;
//   }

  // cout << "load1 : generate() = " << generate()->getValue() << endl;
  // cout << "load1 : generate() = " << generate()->getValue() << endl;
}

namespace {
static int kStaticData = 0;
int kData = 0;
}

extern "C" __attribute__((visibility("default"))) MyClass* load_1() {
  kStaticData++;
  kData++;

  MyClass *c2 = load_2();
  cout << "c2 : " << c2 << " "  << c2->getValue() << endl;
  MyClass1 *c1 = dynamic_cast<MyClass1 *>(c2);
  return c1;
  // return new MyClass1(kStaticData + kData);
}