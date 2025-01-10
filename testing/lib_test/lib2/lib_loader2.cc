#include <iostream>
#include <dlfcn.h>
#include "../lib/generator_lib.h"
#include "lib_loader2.h"
using namespace std;

extern "C" __attribute__((visibility("default"))) void load2() {
//   void *handle = dlopen("./generator.so", RTLD_LAZY);
//   using GenerateFunc = int (*)();
//   GenerateFunc generator_func = (GenerateFunc)dlsym(handle, "generate");
//   const char *dlsym_error = dlerror();
//   if (dlsym_error) {
//     std::cerr << "无法获取函数地址: " << dlsym_error << std::endl;
//     dlclose(handle);
//     return;
//   }

  cout << "load2 : generate() = " << generate()->getValue() << endl;
  cout << "load2 : generate() = " << generate()->getValue() << endl;
}


namespace {
static int kStaticData = 0;
int kData = 0;
}

class  MyClass2 : public MyClass {
 public:
  MyClass2(int value) {
    value_ = value;
  }
public:
  int getValue() override {
    return value_;
  }

  void setValue(int num) override {
    value_ = num;
  }
 protected:
  int value_;
};

extern "C" __attribute__((visibility("default"))) MyClass* load_2() {
  kStaticData++;
  kData++;
  return new MyClass2(kStaticData + kData);
}