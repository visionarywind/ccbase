#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义红黑树节点颜色
typedef enum { RED, BLACK } Color;

// 定义内存块结构体
typedef struct MemoryBlock {
    size_t size;
    int is_free;
    struct MemoryBlock *prev;
    struct MemoryBlock *next;
    Color color;
    struct MemoryBlock *left;
    struct MemoryBlock *right;
    struct MemoryBlock *parent;
} MemoryBlock;

// 定义红黑树结构体
typedef struct RBTree {
    MemoryBlock *root;
    MemoryBlock *NIL;
} RBTree;

// 初始化红黑树
void RBTree_Init(RBTree *tree) {
    tree->NIL = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    if (tree->NIL == NULL) {
        fprintf(stderr, "Memory allocation failed for NIL node.\n");
        exit(EXIT_FAILURE);
    }
    tree->NIL->color = BLACK;
    tree->NIL->left = NULL;
    tree->NIL->right = NULL;
    tree->NIL->parent = NULL;
    tree->root = tree->NIL;
}

// 左旋操作
void LeftRotate(RBTree *tree, MemoryBlock *x) {
    MemoryBlock *y = x->right;
    x->right = y->left;
    if (y->left != tree->NIL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->NIL) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

// 右旋操作
void RightRotate(RBTree *tree, MemoryBlock *y) {
    MemoryBlock *x = y->left;
    y->left = x->right;
    if (x->right != tree->NIL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == tree->NIL) {
        tree->root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

// 插入修复
void RBInsertFixup(RBTree *tree, MemoryBlock *z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            MemoryBlock *y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    LeftRotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                RightRotate(tree, z->parent->parent);
            }
        } else {
            MemoryBlock *y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    RightRotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                LeftRotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

// 插入节点
void RBInsert(RBTree *tree, MemoryBlock *z) {
    MemoryBlock *y = tree->NIL;
    MemoryBlock *x = tree->root;
    while (x != tree->NIL) {
        y = x;
        if (z->size < x->size) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == tree->NIL) {
        tree->root = z;
    } else if (z->size < y->size) {
        y->left = z;
    } else {
        y->right = z;
    }
    z->left = tree->NIL;
    z->right = tree->NIL;
    z->color = RED;
    RBInsertFixup(tree, z);
}

// 查找最小节点
MemoryBlock *TreeMinimum(RBTree *tree, MemoryBlock *x) {
    while (x->left != tree->NIL) {
        x = x->left;
    }
    return x;
}

// 移植操作
void RBTransplant(RBTree *tree, MemoryBlock *u, MemoryBlock *v) {
    if (u->parent == tree->NIL) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

// 删除修复
void RBDeleteFixup(RBTree *tree, MemoryBlock *x) {
    while (x != tree->root && x->color == BLACK) {
        if (x == x->parent->left) {
            MemoryBlock *w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                LeftRotate(tree, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    RightRotate(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                LeftRotate(tree, x->parent);
                x = tree->root;
            }
        } else {
            MemoryBlock *w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                RightRotate(tree, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    LeftRotate(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                RightRotate(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = BLACK;
}

// 删除节点
void RBDelete(RBTree *tree, MemoryBlock *z) {
    MemoryBlock *y = z;
    Color y_original_color = y->color;
    MemoryBlock *x;
    if (z->left == tree->NIL) {
        x = z->right;
        RBTransplant(tree, z, z->right);
    } else if (z->right == tree->NIL) {
        x = z->left;
        RBTransplant(tree, z, z->left);
    } else {
        y = TreeMinimum(tree, z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            RBTransplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        RBTransplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (y_original_color == BLACK) {
        RBDeleteFixup(tree, x);
    }
    free(z);
}

// 查找合适的空闲块
MemoryBlock *FindSuitableBlock(RBTree *tree, size_t size) {
    MemoryBlock *current = tree->root;
    MemoryBlock *suitable = tree->NIL;
    while (current != tree->NIL) {
        if (current->is_free && current->size >= size) {
            if (suitable == tree->NIL || current->size < suitable->size) {
                suitable = current;
            }
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return suitable;
}

// 初始化内存池
void *InitMemoryPool(RBTree *tree, size_t pool_size) {
    void *pool = malloc(pool_size);
    if (pool == NULL) {
        fprintf(stderr, "Memory allocation failed for memory pool.\n");
        return NULL;
    }
    MemoryBlock *block = (MemoryBlock *)pool;
    block->size = pool_size - sizeof(MemoryBlock);
    block->is_free = 1;
    block->prev = NULL;
    block->next = NULL;
    RBInsert(tree, block);
    return (char *)pool + sizeof(MemoryBlock);
}

// 分配内存
void *AllocateMemory(RBTree *tree, size_t size) {
    MemoryBlock *block = FindSuitableBlock(tree, size);
    if (block == tree->NIL) {
        return NULL;
    }
    if (block->size > size + sizeof(MemoryBlock)) {
        // 分割内存块
        MemoryBlock *new_block = (MemoryBlock *)((char *)block + sizeof(MemoryBlock) + size);
        new_block->size = block->size - size - sizeof(MemoryBlock);
        new_block->is_free = 1;
        new_block->prev = block;
        new_block->next = block->next;
        if (block->next != NULL) {
            block->next->prev = new_block;
        }
        block->next = new_block;
        block->size = size;
        RBInsert(tree, new_block);
    }
    block->is_free = 0;
    RBDelete(tree, block);
    return (char *)block + sizeof(MemoryBlock);
}

// 释放内存
void FreeMemory(RBTree *tree, void *ptr) {
    MemoryBlock *block = (MemoryBlock *)((char *)ptr - sizeof(MemoryBlock));
    block->is_free = 1;
    // 合并相邻空闲块
    if (block->prev != NULL && block->prev->is_free) {
        MemoryBlock *prev_block = block->prev;
        prev_block->size += block->size + sizeof(MemoryBlock);
        prev_block->next = block->next;
        if (block->next != NULL) {
            block->next->prev = prev_block;
        }
        RBDelete(tree, block);
        block = prev_block;
    }
    if (block->next != NULL && block->next->is_free) {
        MemoryBlock *next_block = block->next;
        block->size += next_block->size + sizeof(MemoryBlock);
        block->next = next_block->next;
        if (next_block->next != NULL) {
            next_block->next->prev = block;
        }
        RBDelete(tree, next_block);
    }
    RBInsert(tree, block);
}

// 释放内存池
void FreeMemoryPool(RBTree *tree, void *pool) {
    free(pool);
    free(tree->NIL);
}

int main() {
    RBTree tree;
    RBTree_Init(&tree);
    void *pool = InitMemoryPool(&tree, 1024);
    if (pool == NULL) {
        printf("Failed to initialize memory pool.\n");
        return 1;
    }
    void *ptr1 = AllocateMemory(&tree, 256);
    if (ptr1 == NULL) {
        printf("Failed to allocate memory.\n");
    } else {
        printf("Allocated memory at %p\n", ptr1);
    }
    FreeMemory(&tree, ptr1);
    FreeMemoryPool(&tree, pool);
    return 0;
}