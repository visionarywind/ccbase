#include <iostream>
#include <dlfcn.h>
using namespace std;

extern "C" __attribute__((visibility("default"))) void load() {
  void *handle = dlopen("./generator.so", RTLD_LAZY);
  using GenerateFunc = int (*)();
  GenerateFunc generator_func = (GenerateFunc)dlsym(handle, "generate");
  const char *dlsym_error = dlerror();
  if (dlsym_error) {
    std::cerr << "无法获取函数地址: " << dlsym_error << std::endl;
    dlclose(handle);
    return;
  }

  cout << "load2 : generate() = " << generator_func() << endl;
  cout << "load2 : generate() = " << generator_func() << endl;
}