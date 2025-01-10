#include <iostream>
// #include <memory>
#include <dlfcn.h>
using namespace std;

int main() {
  int *p = (int *)malloc(sizeof(int) * 100000);
  *p = 10;
  std::cout << "Value of p: " << *p << std::endl;

  //   void *handle = dlopen("./m.so", RTLD_LAZY);
  //   using LoadFunc = void *(*)(size_t);
  //   LoadFunc load_func1 = (LoadFunc)dlsym(handle, "malloc");
  //   const char *dlsym_error = dlerror();
  //   if (dlsym_error) {
  //     std::cerr << "无法获取函数地址: " << dlsym_error << std::endl;
  //     dlclose(handle);
  //     return 1;
  //   }
  //   void *ptr = load_func1(1);

  return 0;
}
