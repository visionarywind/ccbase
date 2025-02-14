#include "lib.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

// #include <assert.h>

#include <inttypes.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
typedef char byte_t;

#define USE_JE_MALLOC
#ifdef USE_JE_MALLOC
#include <jemalloc/jemalloc.h>
#endif

#define USE_LIB_UNWIND

#ifdef USE_LIB_UNWIND
#include <libunwind.h>
#endif

#include "ms_print.h"

#include "init.h"

// #include "map.h"

#define MEMORY_SIZE 4096

#ifdef USE_LIB_UNWIND
void print_backtrace() {
  ExecStack *exec_stack = (ExecStack *)real_malloc(sizeof(ExecStack));
  int frames = unw_backtrace(exec_stack->buffer, MAX_STACK_FRAMES);
  exec_stack->frames = frames;
  AppendExecStack(exec_stack);
}
#endif

////////////////////////

EXPORT void *malloc(size_t size) {
  // malloc_printf("lib malloc %ld\n", size);
  if (real_malloc) {
#ifdef USE_LIB_UNWIND
    print_backtrace();
#endif
    return real_malloc(size);
  }

  if (size < MEMORY_SIZE) {
    size = MEMORY_SIZE;
  }
  void *mapped_memory = mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (mapped_memory == MAP_FAILED) {
    return NULL;
  }
  return mapped_memory;
}

/*
EXPORT void *calloc(size_t nmemb, size_t size) {
  if (size < MEMORY_SIZE) {
    size = MEMORY_SIZE;
  }
  void *mapped_memory = mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (mapped_memory == MAP_FAILED) {
    return NULL;
  }
  return mapped_memory;
}

EXPORT
int posix_memalign(void **ptr, size_t alignment, size_t size) { return (int)alignment; }

EXPORT void *realloc(void *ptr, size_t size) {
  if (size < MEMORY_SIZE) {
    size = MEMORY_SIZE;
  }
  void *mapped_memory = mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (mapped_memory == MAP_FAILED) {
    return NULL;
  }
  return mapped_memory;
}

EXPORT void *aligned_alloc(size_t alignment, size_t size) {
  if (size < MEMORY_SIZE) {
    size = MEMORY_SIZE;
  }
  void *mapped_memory = mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (mapped_memory == MAP_FAILED) {
    return NULL;
  }
  return mapped_memory;
}
*/

EXPORT void free(void *ptr) {
  // malloc_printf("lib free %p\n", ptr);
  if (real_free) {
#ifdef USE_LIB_UNWIND
    print_backtrace();
#endif
    real_free(ptr);
  } else {
    munmap(ptr, MEMORY_SIZE);
  }
}

/*
// Not standard api.
EXPORT
void *mallocx(size_t size, int flags) {
  malloc_printf("mallocx %ld\n", size);
  return malloc(size);
}

EXPORT
void *rallocx(void *ptr, size_t size, int flags) { return realloc(ptr, size); }

EXPORT
size_t xallocx(void *ptr, size_t size, size_t extra, int flags) { return size; }

EXPORT
size_t sallocx(void *ptr, int flags) { return 0; }

EXPORT
void dallocx(void *ptr, int flags) { free(ptr); }

EXPORT
void sdallocx(void *ptr, size_t size, int flags) { free(ptr); }
*/

// C++ api
void *operator new(size_t size) {
  // malloc_printf("lib new size : %ld\n", size);
  return malloc(size);
}

void *operator new[](size_t size) {
    // malloc_printf("lib new[] size : %ld\n", size);
    return malloc(size);
}

void operator delete(void *ptr) noexcept {
    // malloc_printf("lib delete ptr : %p\n", ptr);
    return free(ptr);
}

void operator delete[](void *ptr) noexcept {
    // malloc_printf("lib delete[] ptr : %p\n", ptr);
    return free(ptr);
}