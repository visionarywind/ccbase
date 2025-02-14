#pragma once

#include "concurrent_queue.h"

////////////////////////
typedef void *(*malloc_libc)(size_t);
typedef void *(*calloc_libc)(size_t nmemb, size_t size);
typedef void *(*realloc_libc)(void *ptr, size_t size);
typedef void *(*aligned_alloc_libc)(size_t alignment, size_t size);
typedef void (*free_libc)(void *ptr);

#define EXTERNC extern "C"

EXTERNC void *handle_libc;
EXTERNC malloc_libc real_malloc;
EXTERNC calloc_libc real_calloc;
EXTERNC realloc_libc real_realloc;
EXTERNC aligned_alloc_libc real_aligned_alloc;
EXTERNC free_libc real_free;

#define MAX_STACK_FRAMES 128
typedef struct {
  void *buffer[MAX_STACK_FRAMES];
  int frames;
} ExecStack;

EXTERNC void AppendExecStack(ExecStack *exec_stack);

EXTERNC void InitLib();