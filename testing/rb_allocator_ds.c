#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

// 红黑树节点颜色
#define RED 0
#define BLACK 1

// 内存块最小大小（必须足够容纳ChunkHeader）
#define MIN_BLOCK_SIZE (sizeof(ChunkHeader) * 2)

// 红黑树节点结构
typedef struct RBNode {
    struct RBNode *parent;
    struct RBNode *left;
    struct RBNode *right;
    int color;
    size_t max_size; // 子树中最大块大小
    void *start;     // 内存块起始地址
    size_t size;     // 内存块大小（包含头）
} RBNode;

// 内存块头结构
typedef struct ChunkHeader {
    RBNode node;      // 红黑树节点
    int is_free;      // 是否空闲
} ChunkHeader;

static RBNode *root = NULL;
static RBNode nil = { &nil, &nil, &nil, BLACK, 0, NULL, 0 };

#define NIL (&nil)

// 工具宏：通过成员指针获取结构体指针
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

// 初始化红黑树
void rb_init() {
    root = NIL;
}

// 红黑树左旋
void rb_left_rotate(RBNode *x) {
    RBNode *y = x->right;
    x->right = y->left;
    if (y->left != NIL)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;

    // 更新max_size
    x->max_size = x->size;
    if (x->left != NIL && x->left->max_size > x->max_size)
        x->max_size = x->left->max_size;
    if (x->right != NIL && x->right->max_size > x->max_size)
        x->max_size = x->right->max_size;
    y->max_size = y->size;
    if (y->left != NIL && y->left->max_size > y->max_size)
        y->max_size = y->left->max_size;
    if (y->right != NIL && y->right->max_size > y->max_size)
        y->max_size = y->right->max_size;
}

// 红黑树右旋
void rb_right_rotate(RBNode *y) {
    RBNode *x = y->left;
    y->left = x->right;
    if (x->right != NIL)
        x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == NIL)
        root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;

    // 更新max_size
    y->max_size = y->size;
    if (y->left != NIL && y->left->max_size > y->max_size)
        y->max_size = y->left->max_size;
    if (y->right != NIL && y->right->max_size > y->max_size)
        y->max_size = y->right->max_size;
    x->max_size = x->size;
    if (x->left != NIL && x->left->max_size > x->max_size)
        x->max_size = x->left->max_size;
    if (x->right != NIL && x->right->max_size > x->max_size)
        x->max_size = x->right->max_size;
}

// 红黑树插入修正
void rb_insert_fixup(RBNode *z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode *y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    rb_left_rotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rb_right_rotate(z->parent->parent);
            }
        } else {
            RBNode *y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rb_right_rotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rb_left_rotate(z->parent->parent);
            }
        }
    }
    root->color = BLACK;
}

// 插入节点到红黑树
void rb_insert(RBNode *z) {
    RBNode *y = NIL;
    RBNode *x = root;
    while (x != NIL) {
        y = x;
        if (z->start < x->start)
            x = x->left;
        else
            x = x->right;
    }
    z->parent = y;
    if (y == NIL)
        root = z;
    else if (z->start < y->start)
        y->left = z;
    else
        y->right = z;
    z->left = NIL;
    z->right = NIL;
    z->color = RED;
    
    // 更新max_size
    RBNode *current = z;
    while (current != NIL) {
        size_t max = current->size;
        if (current->left != NIL && current->left->max_size > max)
            max = current->left->max_size;
        if (current->right != NIL && current->right->max_size > max)
            max = current->right->max_size;
        if (current->max_size == max)
            break;
        current->max_size = max;
        current = current->parent;
    }
    
    rb_insert_fixup(z);
}

// 查找最小节点
RBNode *rb_minimum(RBNode *x) {
    while (x->left != NIL)
        x = x->left;
    return x;
}

// 红黑树删除修正
void rb_delete_fixup(RBNode *x) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            RBNode *w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rb_left_rotate(x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rb_right_rotate(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                rb_left_rotate(x->parent);
                x = root;
            }
        } else {
            RBNode *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rb_right_rotate(x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    rb_left_rotate(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rb_right_rotate(x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
}

// 从红黑树删除节点
void rb_delete(RBNode *z) {
    RBNode *y = z;
    RBNode *x;
    int y_original_color = y->color;
    
    if (z->left == NIL) {
        x = z->right;
        if (z->parent == NIL)
            root = z->right;
        else if (z == z->parent->left)
            z->parent->left = z->right;
        else
            z->parent->right = z->right;
        z->right->parent = z->parent;
    } else if (z->right == NIL) {
        x = z->left;
        if (z->parent == NIL)
            root = z->left;
        else if (z == z->parent->left)
            z->parent->left = z->left;
        else
            z->parent->right = z->left;
        z->left->parent = z->parent;
    } else {
        y = rb_minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z)
            x->parent = y;
        else {
            if (y->parent == NIL)
                root = y->right;
            else if (y == y->parent->left)
                y->parent->left = y->right;
            else
                y->parent->right = y->right;
            y->right->parent = y->parent;
            y->right = z->right;
            y->right->parent = y;
        }
        if (z->parent == NIL)
            root = y;
        else if (z == z->parent->left)
            z->parent->left = y;
        else
            z->parent->right = y;
        y->parent = z->parent;
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    
    // 更新max_size
    RBNode *current = x->parent;
    while (current != NIL) {
        size_t max = current->size;
        if (current->left != NIL && current->left->max_size > max)
            max = current->left->max_size;
        if (current->right != NIL && current->right->max_size > max)
            max = current->right->max_size;
        if (current->max_size == max)
            break;
        current->max_size = max;
        current = current->parent;
    }
    
    if (y_original_color == BLACK)
        rb_delete_fixup(x);
}

// 初始化堆
void init_heap() {
    void *start = 。/sbrk(0);
    void *ptr = sbrk(1024 * 1024); // 1MB初始堆
    if (ptr == (void *)-1) {
        perror("sbrk");
        exit(1);
    }
    
    ChunkHeader *chunk = (ChunkHeader *)start;
    chunk->node.start = start;
    chunk->node.size = 1024 * 1024;
    chunk->node.max_size = 1024 * 1024;
    chunk->is_free = 1;
    rb_insert(&chunk->node);
}

// 查找最佳适配块
RBNode *find_best_fit(size_t size) {
    RBNode *current = root;
    RBNode *best = NIL;
    
    while (current != NIL) {
        if (current->size >= size) {
            best = current;
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return best;
}

// 分配内存
void *my_malloc(size_t size) {
    if (size == 0)
        return NULL;
    
    // 计算需要的内存块大小（包含头结构）
    size_t required_size = size + sizeof(ChunkHeader);
    if (required_size < MIN_BLOCK_SIZE)
        required_size = MIN_BLOCK_SIZE;
    
    RBNode *node = find_best_fit(required_size);
    if (node == NIL) {
        // 内存不足，需要扩展堆（此处简化处理）
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }
    
    // 从红黑树中删除该节点
    rb_delete(node);
    ChunkHeader *chunk = container_of(node, ChunkHeader, node);
    chunk->is_free = 0;
    
    // 检查是否需要分割块
    size_t remaining_size = node->size - required_size;
    if (remaining_size >= MIN_BLOCK_SIZE) {
        // 创建新块
        ChunkHeader *new_chunk = (ChunkHeader *)((char *)node->start + required_size);
        new_chunk->node.start = (void *)new_chunk;
        new_chunk->node.size = remaining_size;
        new_chunk->node.max_size = remaining_size;
        new_chunk->is_free = 1;
        
        // 插入新块到红黑树
        rb_insert(&new_chunk->node);
        
        // 更新原块大小
        node->size = required_size;
    }
    
    // 返回分配的内存地址（跳过头结构）
    return (void *)((char *)node->start + sizeof(ChunkHeader));
}

// 合并相邻块
void merge_blocks(ChunkHeader *chunk) {
    // 合并前一个块
    RBNode *current = &chunk->node;
    RBNode *prev = current->parent;
    while (prev != NIL && prev->right != current)
        prev = prev->parent;
    
    if (prev != NIL) {
        ChunkHeader *prev_chunk = container_of(prev, ChunkHeader, node);
        if (prev_chunk->is_free &&
            (char *)prev_chunk + prev->size == (char *)chunk) {
            rb_delete(prev);
            prev->size += chunk->node.size;
            chunk = prev_chunk;
        }
    }
    
    // 合并后一个块
    RBNode *next_node = rb_minimum(current->right);
    if (next_node != NIL) {
        ChunkHeader *next_chunk = container_of(next_node, ChunkHeader, node);
        if (next_chunk->is_free &&
            (char *)chunk + chunk->node.size == (char *)next_chunk) {
            rb_delete(next_node);
            chunk->node.size += next_node->size;
        }
    }
    
    // 插入合并后的块
    chunk->is_free = 1;
    rb_insert(&chunk->node);
}

// 释放内存
void my_free(void *ptr) {
    if (ptr == NULL)
        return;
    
    ChunkHeader *chunk = (ChunkHeader *)((char *)ptr - sizeof(ChunkHeader));
    if (!chunk->is_free) {
        chunk->is_free = 1;
        merge_blocks(chunk);
    }
}

// 测试用例
int main() {
    init_heap();
    
    void *p1 = my_malloc(100);
    void *p2 = my_malloc(200);
    void *p3 = my_malloc(300);
    
    printf("Allocated: %p, %p, %p\n", p1, p2, p3);
    
    my_free(p2);
    my_free(p3);
    my_free(p1);
    
    printf("Memory freed\n");
    
    return 0;
}