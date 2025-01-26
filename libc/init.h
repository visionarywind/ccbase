#pragma once

#include "concurrent_queue.h"

////////////////////////
typedef void *(*malloc_libc)(size_t);
typedef void *(*calloc_libc)(size_t nmemb, size_t size);
typedef void *(*realloc_libc)(void *ptr, size_t size);
typedef void *(*aligned_alloc_libc)(size_t alignment, size_t size);
typedef void (*free_libc)(void *ptr);

extern "C" void *handle_libc;
extern "C" malloc_libc real_malloc;
extern "C" calloc_libc real_calloc;
extern "C" realloc_libc real_realloc;
extern "C" aligned_alloc_libc real_aligned_alloc;
extern "C" free_libc real_free;

extern "C" ConcurrentQueue kConcurrentQueue;

#define MAX_STACK_FRAMES 128
typedef struct {
  void *buffer[MAX_STACK_FRAMES];
  int frames;
} ExecStack;

extern "C" void AppendExecStack(ExecStack *exec_stack);