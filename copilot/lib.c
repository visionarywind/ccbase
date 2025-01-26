#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define MEMORY_POOL_SIZE 1024 * 1024  // 1 MB

static char memory_pool[MEMORY_POOL_SIZE];
static size_t memory_pool_index = 0;

typedef struct block_meta {
  size_t size;
  int free;
  struct block_meta *next;
} block_meta_t;

static block_meta_t *free_list = NULL;

#define META_SIZE sizeof(block_meta_t)

void initialize_memory_manager() {
  free_list = (block_meta_t *)memory_pool;
  free_list->size = MEMORY_POOL_SIZE - META_SIZE;
  free_list->free = 1;
  free_list->next = NULL;
}

void *malloc(size_t size) {
  printf("malloc %ld\n", size);
  if (free_list == NULL) {
    initialize_memory_manager();
  }

  block_meta_t *current = free_list;

  while (current && !(current->free && current->size >= size)) {
    current = current->next;
  }

  if (!current) {
    return NULL;  // Out of memory
  }

  if (current->size > size + META_SIZE) {
    block_meta_t *new_block = (block_meta_t *)((char *)current + META_SIZE + size);
    new_block->size = current->size - size - META_SIZE;
    new_block->free = 1;
    new_block->next = current->next;

    current->size = size;
    current->next = new_block;
  }

  current->free = 0;
  return (char *)current + META_SIZE;
}

void *calloc(size_t num, size_t size) {
  size_t total_size = num * size;
  void *ptr = malloc(total_size);
  if (ptr) {
    memset(ptr, 0, total_size);
  }
  return ptr;
}

void *realloc(void *ptr, size_t size) {
  if (!ptr) {
    return malloc(size);
  }

  block_meta_t *block = (block_meta_t *)((char *)ptr - META_SIZE);
  if (block->size >= size) {
    return ptr;  // Enough space already allocated
  }

  void *new_ptr = malloc(size);
  if (new_ptr) {
    memcpy(new_ptr, ptr, block->size);
    block->free = 1;  // Mark old block as free
  }

  return new_ptr;
}

void free(void *ptr) {
  if (!ptr) {
    return;
  }

  block_meta_t *block = (block_meta_t *)((char *)ptr - META_SIZE);
  block->free = 1;

  // Coalesce adjacent free blocks
  block_meta_t *current = free_list;
  while (current) {
    if (current->free && current->next && current->next->free) {
      current->size += META_SIZE + current->next->size;
      current->next = current->next->next;
    }
    current = current->next;
  }
}