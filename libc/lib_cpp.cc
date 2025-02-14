
#include "lib.h"
#include "ms_print.h"

// C++ api
void *operator new(size_t size) {
#ifdef LIB_TRACING
  malloc_printf("lib new size : %ld\n", size);
#endif
  return malloc(size);
}

void *operator new[](size_t size) {
#ifdef LIB_TRACING
  malloc_printf("lib new[] size : %ld\n", size);
#endif
  return malloc(size);
}

void operator delete(void *ptr) noexcept {
#ifdef LIB_TRACING
  malloc_printf("lib delete ptr : %p\n", ptr);
#endif
  return free(ptr);
}

void operator delete[](void *ptr) noexcept {
#ifdef LIB_TRACING
  malloc_printf("lib delete[] ptr : %p\n", ptr);
#endif
  return free(ptr);
}