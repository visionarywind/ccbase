
#include "lib.h"
#include "ms_print.h"

// C++ api
void *operator new(size_t size) {
  malloc_printf("lib new size : %ld\n", size);
  return malloc(size);
}

void *operator new[](size_t size) {
  malloc_printf("lib new[] size : %ld\n", size);
  return malloc(size);
}

void operator delete(void *ptr) noexcept {
  malloc_printf("lib delete ptr : %p\n", ptr);
  return free(ptr);
}

void operator delete[](void *ptr) noexcept {
  malloc_printf("lib delete[] ptr : %p\n", ptr);
  return free(ptr);
}