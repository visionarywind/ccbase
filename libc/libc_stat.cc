#include "lib.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>

#ifdef USE_LIB_UNWIND
#include <libunwind.h>
#endif

#include "ms_print.h"

#include "init.h"

#ifdef USE_LIB_UNWIND
void print_backtrace() {
#define MAX_STACK_FRAMES 10
  void *buffer[MAX_STACK_FRAMES];
  int frames = unw_backtrace(buffer, MAX_STACK_FRAMES);

  for (int i = 0; i < frames; i++) {
    malloc_printf("Frame %d: 0x%lx\n", i, (long)buffer[i]);
  }
}
#endif

////////////////////////
EXPORT void *ALIAS(malloc) malloc_with_stat(size_t size) {
  malloc_printf("libc - malloc %ld\n", size);
#ifdef USE_LIB_UNWIND
  print_backtrace();
#endif
  if (real_malloc) {
    printf("real malloc is called : %p\n", real_malloc);
    return real_malloc(size);
  }
  // Exception routine.
  malloc_printf("libc - malloc failed\n");
  return NULL;
}

EXPORT void *ALIAS(calloc) calloc_with_stat(size_t nmemb, size_t size) {
  malloc_printf("libc - malloc %ld\n", size);
  return real_calloc(nmemb, size);
}

EXPORT void *ALIAS(realloc) realloc_with_stat(void *ptr, size_t size) {
  malloc_printf("libc - realloc %p %ld\n", ptr, size);
  return real_realloc(ptr, size);
}

EXPORT void *ALIAS(aligned_alloc) aligned_alloc_with_stat(size_t alignment, size_t size) {
  malloc_printf("libc - aligned_alloc %ld %ld\n", alignment, size);
  return real_aligned_alloc(alignment, size);
}

EXPORT void ALIAS(free) free_with_stat(void *ptr) {
  malloc_printf("libc - free %p\n", ptr);
  real_free(ptr);
}