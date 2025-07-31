#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

#define MIN_BLOCK_SIZE 24
#define ALIGNMENT 8
#define GUARD_TAG 0xABABABAB

typedef enum { RED = 0, BLACK } rb_color;

typedef struct {
  uint32_t size : 30;
  uint32_t alloc : 1;
  uint32_t color : 1;
} block_meta;

typedef struct free_block {
  block_meta header;
  struct free_block *left;
  struct free_block *right;
  struct free_block *parent;
} free_block;

static free_block *rb_root = NULL;
static void *heap_start = NULL;
static void *heap_end = NULL;

static inline size_t align_up(size_t size) { return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1); }

static inline size_t get_total_size(size_t size) {
  size_t total = align_up(size + sizeof(block_meta) * 2);
  return total < MIN_BLOCK_SIZE ? MIN_BLOCK_SIZE : total;
}

static inline block_meta *get_footer(free_block *block) {
  return (block_meta *)((char *)block + block->header.size - sizeof(block_meta));
}

static inline free_block *get_next_block(free_block *block) {
  return (free_block *)((char *)block + block->header.size);
}

static inline free_block *get_prev_block(free_block *block) {
  block_meta *prev_footer = (block_meta *)((char *)block - sizeof(block_meta));
  return (free_block *)((char *)prev_footer - prev_footer->size + sizeof(block_meta));
}

static void init_block(free_block *block, size_t size, int alloc) {
  block->header.size = size;
  block->header.alloc = alloc;
  block->header.color = alloc ? 0 : RED;

  block_meta *footer = get_footer(block);
  footer->size = size;
  footer->alloc = alloc;
}

static void rb_rotate_left(free_block *x) {
  free_block *y = x->right;
  x->right = y->left;
  if (y->left) y->left->parent = x;
  y->parent = x->parent;

  if (!x->parent)
    rb_root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

  y->left = x;
  x->parent = y;
}

static void rb_rotate_right(free_block *x) {
  free_block *y = x->left;
  x->left = y->right;
  if (y->right) y->right->parent = x;
  y->parent = x->parent;

  if (!x->parent)
    rb_root = y;
  else if (x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;

  y->right = x;
  x->parent = y;
}

static void rb_insert_fixup(free_block *z) {
  while (z->parent && z->parent->header.color == RED) {
    if (z->parent == z->parent->parent->left) {
      free_block *y = z->parent->parent->right;
      if (y && y->header.color == RED) {
        z->parent->header.color = BLACK;
        y->header.color = BLACK;
        z->parent->parent->header.color = RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right) {
          z = z->parent;
          rb_rotate_left(z);
        }
        z->parent->header.color = BLACK;
        z->parent->parent->header.color = RED;
        rb_rotate_right(z->parent->parent);
      }
    } else {
      free_block *y = z->parent->parent->left;
      if (y && y->header.color == RED) {
        z->parent->header.color = BLACK;
        y->header.color = BLACK;
        z->parent->parent->header.color = RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z = z->parent;
          rb_rotate_right(z);
        }
        z->parent->header.color = BLACK;
        z->parent->parent->header.color = RED;
        rb_rotate_left(z->parent->parent);
      }
    }
  }
  rb_root->header.color = BLACK;
}

static void rb_insert(free_block *z) {
  free_block *y = NULL;
  free_block *x = rb_root;

  while (x) {
    y = x;
    if (z->header.size < x->header.size)
      x = x->left;
    else
      x = x->right;
  }

  z->parent = y;
  if (!y)
    rb_root = z;
  else if (z->header.size < y->header.size)
    y->left = z;
  else
    y->right = z;

  z->left = z->right = NULL;
  z->header.color = RED;
  rb_insert_fixup(z);
}

static void rb_delete_fixup(free_block *x) {
  while (x != rb_root && (!x || x->header.color == BLACK)) {
    if (x == x->parent->left) {
      free_block *w = x->parent->right;
      if (w->header.color == RED) {
        w->header.color = BLACK;
        x->parent->header.color = RED;
        rb_rotate_left(x->parent);
        w = x->parent->right;
      }
      if ((!w->left || w->left->header.color == BLACK) && (!w->right || w->right->header.color == BLACK)) {
        w->header.color = RED;
        x = x->parent;
      } else {
        if (!w->right || w->right->header.color == BLACK) {
          if (w->left) w->left->header.color = BLACK;
          w->header.color = RED;
          rb_rotate_right(w);
          w = x->parent->right;
        }
        w->header.color = x->parent->header.color;
        x->parent->header.color = BLACK;
        if (w->right) w->right->header.color = BLACK;
        rb_rotate_left(x->parent);
        x = rb_root;
      }
    } else {
      free_block *w = x->parent->left;
      if (w->header.color == RED) {
        w->header.color = BLACK;
        x->parent->header.color = RED;
        rb_rotate_right(x->parent);
        w = x->parent->left;
      }
      if ((!w->right || w->right->header.color == BLACK) && (!w->left || w->left->header.color == BLACK)) {
        w->header.color = RED;
        x = x->parent;
      } else {
        if (!w->left || w->left->header.color == BLACK) {
          if (w->right) w->right->header.color = BLACK;
          w->header.color = RED;
          rb_rotate_left(w);
          w = x->parent->left;
        }
        w->header.color = x->parent->header.color;
        x->parent->header.color = BLACK;
        if (w->left) w->left->header.color = BLACK;
        rb_rotate_right(x->parent);
        x = rb_root;
      }
    }
  }
  if (x) x->header.color = BLACK;
}

static void rb_delete(free_block *z) {
  free_block *y = z;
  free_block *x;
  rb_color y_original_color = static_cast<rb_color>(y->header.color);

  if (!z->left) {
    x = z->right;
    if (!z->parent)
      rb_root = z->right;
    else if (z == z->parent->left)
      z->parent->left = z->right;
    else
      z->parent->right = z->right;
    if (z->right) z->right->parent = z->parent;
  } else if (!z->right) {
    x = z->left;
    if (!z->parent)
      rb_root = z->left;
    else if (z == z->parent->left)
      z->parent->left = z->left;
    else
      z->parent->right = z->left;
    if (z->left) z->left->parent = z->parent;
  } else {
    y = z->right;
    while (y->left) y = y->left;
    y_original_color = static_cast<rb_color>(y->header.color);
    x = y->right;

    if (y->parent == z) {
      if (x) x->parent = y;
    } else {
      if (x) x->parent = y->parent;
      y->parent->left = x;
      y->right = z->right;
      z->right->parent = y;
    }

    if (!z->parent)
      rb_root = y;
    else if (z == z->parent->left)
      z->parent->left = y;
    else
      z->parent->right = y;

    y->parent = z->parent;
    y->left = z->left;
    z->left->parent = y;
    y->header.color = z->header.color;
  }

  if (y_original_color == BLACK && x) rb_delete_fixup(x);
}

static free_block *rb_search_best_fit(size_t size) {
  free_block *current = rb_root;
  free_block *best = NULL;

  while (current) {
    if (current->header.size >= size) {
      best = current;
      current = current->left;
    } else {
      current = current->right;
    }
  }
  return best;
}

static free_block *expand_heap(size_t size) {
  size_t page_size = sysconf(_SC_PAGESIZE);
  size_t request_size = (size + page_size - 1) / page_size * page_size;

  void *block = sbrk(request_size);
  if (block == (void *)-1) return NULL;

  free_block *new_block = (free_block *)block;
  init_block(new_block, request_size, 0);

  if (!heap_start) heap_start = block;
  heap_end = (char *)block + request_size;

  return new_block;
}

static void *split_block(free_block *block, size_t size) {
  size_t remaining = block->header.size - size;

  if (remaining >= MIN_BLOCK_SIZE) {
    free_block *new_block = (free_block *)((char *)block + size);
    init_block(new_block, remaining, 0);

    rb_insert(new_block);

    block->header.size = size;
    get_footer(block)->size = size;
  }

  block->header.alloc = 1;
  get_footer(block)->alloc = 1;

  return (void *)((char *)block + sizeof(block_meta));
}

static free_block *coalesce_blocks(free_block *block) {
  if (block != heap_start) {
    free_block *prev = get_prev_block(block);
    if (!prev->header.alloc) {
      rb_delete(prev);

      prev->header.size += block->header.size;
      get_footer(prev)->size = prev->header.size;

      block = prev;
    }
  }

  if ((char *)block + block->header.size < (char *)heap_end) {
    free_block *next = get_next_block(block);
    if (!next->header.alloc) {
      rb_delete(next);

      block->header.size += next->header.size;
      get_footer(block)->size = block->header.size;
    }
  }

  return block;
}

void allocator_init() {
  size_t page_size = sysconf(_SC_PAGESIZE);
  heap_start = sbrk(4 * page_size);
  heap_end = (char *)heap_start + 4 * page_size;

  free_block *guard = (free_block *)heap_start;
  init_block(guard, sizeof(block_meta), 1);
  guard->header.size = GUARD_TAG;

  free_block *main_block = (free_block *)((char *)heap_start + sizeof(block_meta));
  size_t main_size = 4 * page_size - 2 * sizeof(block_meta);
  init_block(main_block, main_size, 0);

  rb_root = main_block;
  main_block->left = main_block->right = main_block->parent = NULL;
  main_block->header.color = BLACK;
}

void *my_malloc(size_t size) {
  if (size == 0) return NULL;

  size_t total_size = get_total_size(size);

  free_block *block = rb_search_best_fit(total_size);

  if (!block) {
    block = expand_heap(total_size);
    if (!block) return NULL;
    rb_insert(block);
  }

  rb_delete(block);

  return split_block(block, total_size);
}

void my_free(void *ptr) {
  if (!ptr) return;

  free_block *block = (free_block *)((char *)ptr - sizeof(block_meta));

  if (block->header.size == GUARD_TAG) return;
  if (get_footer(block)->size != block->header.size) return;

  block->header.alloc = 0;
  get_footer(block)->alloc = 0;

  block = coalesce_blocks(block);

  block->left = block->right = block->parent = NULL;
  rb_insert(block);
}

#define TEST
#ifdef TEST
#include <stdio.h>
#include <assert.h>

void test_allocator() {
  allocator_init();

  // 基本分配/释放测试
  void *p1 = my_malloc(100);
  void *p2 = my_malloc(200);
  void *p3 = my_malloc(300);

  assert(p1 != NULL);
  assert(p2 != NULL);
  assert(p3 != NULL);

  my_free(p2);
  my_free(p1);
  my_free(p3);

  void *small = my_malloc(16);
  void *medium = my_malloc(128);
  void *large = my_malloc(1024);

  my_free(medium);

  void *reuse = my_malloc(120);
  assert(reuse == medium);

  my_free(small);
  my_free(large);
  my_free(reuse);

  void *a = my_malloc(64);
  void *b = my_malloc(64);
  void *c = my_malloc(64);

  my_free(b);
  my_free(a);

  free_block *big = (free_block *)((char *)a - sizeof(block_meta));
  assert(big->header.size == 128 + 2 * sizeof(block_meta));

  my_free(c);

  printf("All tests passed!\n");
}

int main() {
  printf("test rb tree!\n");
  test_allocator();
  return 0;
}
#endif