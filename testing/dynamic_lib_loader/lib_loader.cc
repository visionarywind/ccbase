#include <iostream>
#include <dlfcn.h>
#include <thread>
using namespace std;

int main() {
  void *handle = dlopen("./loader1.so", RTLD_LAZY);
  using LoadFunc = void (*)();
  LoadFunc load_func1 = (LoadFunc)dlsym(handle, "load");
  const char *dlsym_error = dlerror();
  if (dlsym_error) {
    std::cerr << "无法获取函数地址: " << dlsym_error << std::endl;
    dlclose(handle);
    return 1;
  }

  cout << "load1() called " << endl;
  load_func1();

  std::thread t1([]() {
    void *handle = dlopen("./loader2.so", RTLD_LAZY);
    using LoadFunc = void (*)();
    LoadFunc load_func2 = (LoadFunc)dlsym(handle, "load");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
      std::cerr << "无法获取函数地址: " << dlsym_error << std::endl;
      dlclose(handle);
      return 1;
    }

    cout << "load2() called " << endl;
    load_func2();
    return 0;
  });
  t1.join();
  return 0;
}