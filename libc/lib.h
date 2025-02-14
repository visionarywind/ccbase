#pragma once
#include <stddef.h>

#define EXPORT extern "C" __attribute__((visibility("default")))
#define ALIAS(name) __attribute__((alias(#name)))

#if defined(__cplusplus)
#  define CXX_THROW noexcept (true)
#else
#  define CXX_THROW
#endif

EXPORT void *malloc(size_t size) CXX_THROW;
EXPORT void *calloc(size_t nmemb, size_t size) CXX_THROW;
EXPORT void *realloc(void *ptr, size_t size) CXX_THROW;
EXPORT void *aligned_alloc(size_t alignment, size_t size) CXX_THROW;
EXPORT void free(void *ptr) CXX_THROW;