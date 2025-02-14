#include <memory.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>

#include "def.h"
#include "init.h"
#include "ms_print.h"

void *handle_libc = NULL;
malloc_libc real_malloc = NULL;
calloc_libc real_calloc = NULL;
realloc_libc real_realloc = NULL;
aligned_alloc_libc real_aligned_alloc = NULL;
free_libc real_free = NULL;

ConcurrentQueue kConcurrentQueue;

volatile bool init_flag = true;

void AppendExecStack(ExecStack *exec_stack) {
  // malloc_printf("append : %d\n", exec_stack->frames);
  concurrent_queue_enqueue(&kConcurrentQueue, exec_stack);
}

void *consumer(void *arg) {
  malloc_printf("real_free : %p, arg : %p\n", real_free, arg);
  static size_t cnt = 0;
  while (init_flag) {
    ExecStack *data = (ExecStack *)concurrent_queue_dequeue(&kConcurrentQueue);
    if (cnt % 10000 == 0) {
      malloc_printf("Received: %d\n", data->frames);
    }
    cnt++;
    (free_libc(arg))(data);
  }
  return NULL;
}

#define GET_REAL_FUNC(func, func_type)                  \
  if (!real_##func) {                                   \
    real_##func = (func_type)dlsym(handle_libc, #func); \
    if (!real_##func) {                                 \
      malloc_printf("dlsym failed: %s\n", dlerror());   \
      dlclose(handle_libc);                             \
      handle_libc = NULL;                               \
      return;                                           \
    }                                                   \
    malloc_printf("%s func : %p\n", #func, real_##func);          \
  }

#undef malloc
__attribute__((constructor)) void init_library() {
  malloc_printf("init library\n");
#ifdef USE_JE_MALLOC
  real_malloc = je_malloc;
  real_calloc = je_calloc;
  real_realloc = je_realloc;
  real_aligned_alloc = je_aligned_alloc;
  real_free = je_free;

  malloc_printf("malloc func : %p\n", real_malloc);
  malloc_printf("calloc func : %p\n", real_calloc);
  malloc_printf("realloc func : %p\n", real_realloc);
  malloc_printf("aligned_alloc func : %p\n", real_aligned_alloc);
  malloc_printf("free func : %p\n", real_free);
#else
#ifdef __APPLE__
  handle_libc = dlopen("libc.dylib", RTLD_LAZY);
#else
  handle_libc = dlopen("libc.so.6", RTLD_LAZY);
#endif
  if (!handle_libc) {
    malloc_printf("dlopen failed: %s\n", dlerror());
    return;
  }

  GET_REAL_FUNC(malloc, malloc_libc);
  GET_REAL_FUNC(calloc, calloc_libc);
  GET_REAL_FUNC(realloc, realloc_libc);
  GET_REAL_FUNC(aligned_alloc, aligned_alloc_libc);
  GET_REAL_FUNC(free, free_libc);
#endif

  concurrent_queue_init(&kConcurrentQueue, real_malloc, real_free);

  pthread_t thread_id;
  // 创建新线程
  int result = pthread_create(&thread_id, NULL, consumer, (void *)real_free);
  if (result == 0) {
    malloc_printf("init backend thread success.\n");
  } else {
    malloc_printf("init backend thread failed.\n");
    init_flag = false;
  }
}

__attribute__((destructor)) void cleanup_library() {
  malloc_printf("Shared library is being cleaned up.\n");
#ifndef USE_JE_MALLOC
  if (handle_libc) {
    init_flag = false;
    ExecStack *stack = (ExecStack *)real_malloc(sizeof(ExecStack));
    AppendExecStack(stack);
    concurrent_queue_destroy(&kConcurrentQueue);
    real_free(stack);
    dlclose(handle_libc);
    handle_libc = NULL;
  }
#endif
}