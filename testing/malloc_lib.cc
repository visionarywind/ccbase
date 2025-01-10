#include <dlfcn.h>
#include <iostream>

typedef void *(*malloc_ptr)(size_t size);

extern "C" __attribute__((visibility("default"))) void *malloc(size_t size) {
  malloc_ptr _malloc;
  std::cout << "malloc : " << size << std::endl;
  _malloc = (malloc_ptr)dlsym(RTLD_NEXT, "malloc");  // look up original malloc()
  return _malloc(size);                              // and invoke it
}