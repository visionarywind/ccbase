#pragma once
#include <stddef.h>

#define EXPORT extern "C" __attribute__((visibility("default")))
#define ALIAS(name) __attribute__((alias(#name)))

EXPORT void *malloc(size_t size);
// EXPORT void *calloc(size_t nmemb, size_t size);
// EXPORT void *realloc(void *ptr, size_t size);
// EXPORT void *aligned_alloc(size_t alignment, size_t size);
EXPORT void free(void *ptr);