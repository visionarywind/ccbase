
#include <cstddef>
#include <cstdint>
#include <cstdlib>

// 内存块结构体
struct Block {
    size_t size;          // 可用空间大小
    bool is_free;         // 是否空闲
    void* start_addr;     // 可用空间起始地址

    // 链表指针
    Block* prev;
    Block* next;

    // 红黑树指针
    Block* parent;
    Block* left;
    Block* right;
    bool is_red;          // 颜色标志

    Block() : size(0), is_free(false), start_addr(nullptr),
             prev(nullptr), next(nullptr),
             parent(nullptr), left(nullptr), right(nullptr), is_red(false) {}
};

// 内存池类
class MemoryPool {
private:
    void* pool_start;      // 内存池起始地址
    size_t pool_size;      // 内存池总大小
    Block* free_tree_root; // 红黑树根节点
    Block* free_list_head; // 空闲链表头节点

    // 红黑树操作
    void rotateLeft(Block* node);
    void rotateRight(Block* node);
    void fixInsert(Block* node);
    void fixDelete(Block* node);
    Block* findNode(size_t size);
    void transplant(Block* u, Block* v);
    
    // 链表操作
    void insertList(Block* node);
    void removeList(Block* node);
    void mergeAdjacent(Block* block);

public:
    MemoryPool(void* buffer, size_t size);
    ~MemoryPool();

    void* allocate(size_t size);
    void deallocate(void* ptr);
};

// 内存池构造函数
MemoryPool::MemoryPool(void* buffer, size_t size) 
    : pool_start(buffer), pool_size(size), free_tree_root(nullptr), free_list_head(nullptr) {
    
    // 初始化第一个块
    Block* initial = static_cast<Block*>(buffer);
    initial->size = size - sizeof(Block);
    initial->is_free = true;
    initial->start_addr = reinterpret_cast<char*>(buffer) + sizeof(Block);
    
    // 初始化链表
    initial->prev = initial;
    initial->next = initial;
    free_list_head = initial;
    
    // 初始化红黑树
    initial->parent = nullptr;
    initial->left = nullptr;
    initial->right = nullptr;
    initial->is_red = false;
    free_tree_root = initial;
}

// 内存池析构函数
MemoryPool::~MemoryPool() {
    free(pool_start);
}

// 左旋操作
void MemoryPool::rotateLeft(Block* node) {
    Block* right = node->right;
    node->right = right->left;
    
    if (right->left != nullptr) {
        right->left->parent = node;
    }
    
    right->parent = node->parent;
    
    if (node->parent == nullptr) {
        free_tree_root = right;
    } else if (node == node->parent->left) {
        node->parent->left = right;
    } else {
        node->parent->right = right;
    }
    
    right->left = node;
    node->parent = right;
}

// 右旋操作
void MemoryPool::rotateRight(Block* node) {
    Block* left = node->left;
    node->left = left->right;
    
    if (left->right != nullptr) {
        left->right->parent = node;
    }
    
    left->parent = node->parent;
    
    if (node->parent == nullptr) {
        free_tree_root = left;
    } else if (node == node->parent->right) {
        node->parent->right = left;
    } else {
        node->parent->left = left;
    }
    
    left->right = node;
    node->parent = left;
}

// 插入修复
void MemoryPool::fixInsert(Block* node) {
    while (node->parent && node->parent->is_red) {
        if (node->parent == node->parent->parent->left) {
            Block* uncle = node->parent->parent->right;
            if (uncle && uncle->is_red) {
                node->parent->is_red = false;
                uncle->is_red = false;
                node->parent->parent->is_red = true;
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    rotateLeft(node);
                }
                node->parent->is_red = false;
                node->parent->parent->is_red = true;
                rotateRight(node->parent->parent);
            }
        } else {
            Block* uncle = node->parent->parent->left;
            if (uncle && uncle->is_red) {
                node->parent->is_red = false;
                uncle->is_red = false;
                node->parent->parent->is_red = true;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rotateRight(node);
                }
                node->parent->is_red = false;
                node->parent->parent->is_red = true;
                rotateLeft(node->parent->parent);
            }
        }
    }
    free_tree_root->is_red = false;
}

// 查找节点
Block* MemoryPool::findNode(size_t size) {
    Block* current = free_tree_root;
    Block* best = nullptr;
    
    while (current) {
        if (current->size >= size) {
            best = current;
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return best;
}

// 内存分配
void* MemoryPool::allocate(size_t size) {
    if (size == 0) return nullptr;
    
    Block* best = findNode(size);
    if (!best) return nullptr;
    
    // 分割块
    if (best->size > size + sizeof(Block)) {
        // 创建新块
        Block* new_block = reinterpret_cast<Block*>(
            reinterpret_cast<char*>(best->start_addr) + size
        );
        
        new_block->size = best->size - size - sizeof(Block);
        new_block->is_free = true;
        new_block->start_addr = reinterpret_cast<char*>(best->start_addr) + size + sizeof(Block);
        
        // 更新原块
        removeList(best);
        removeList(new_block); // 需要实现红黑树删除
        
        best->size = size;
        best->is_free = false;
        
        insertList(best);
        insertList(new_block);
    } else {
        best->is_free = false;
        removeList(best);
    }
    
    return best->start_addr;
}

// 内存释放
void MemoryPool::deallocate(void* ptr) {
    if (!ptr) return;
    
    Block* block = static_cast<Block*>(
        reinterpret_cast<char*>(ptr) - sizeof(Block)
    );
    
    block->is_free = true;
    insertList(block);
    mergeAdjacent(block);
}

// 链表插入
void MemoryPool::insertList(Block* node) {
    if (!free_list_head) {
        free_list_head = node;
        node->prev = node;
        node->next = node;
    } else {
        node->prev = free_list_head->prev;
        node->next = free_list_head;
        free_list_head->prev->next = node;
        free_list_head->prev = node;
    }
}

// 链表删除
void MemoryPool::removeList(Block* node) {
    if (node->prev == node) {
        free_list_head = nullptr;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if (node == free_list_head) {
            free_list_head = node->next;
        }
    }
}

// 合并相邻块
void MemoryPool::mergeAdjacent(Block* block) {
    Block* current = block;
    
    // 检查前驱
    if (current->prev != current && current->prev->is_free) {
        if (reinterpret_cast<char*>(current->prev->start_addr) + current->prev->size + sizeof(Block) == 
            reinterpret_cast<char*>(current->start_addr)) {
            
            // 合并前驱
            Block* prev = current->prev;
            removeList(prev);
            prev->size += current->size + sizeof(Block);
            current = prev;
        }
    }
    
    // 检查后继
    if (current->next != current && current->next->is_free) {
        if (reinterpret_cast<char*>(current->start_addr) + current->size + sizeof(Block) == 
            reinterpret_cast<char*>(current->next->start_addr)) {
            
            // 合并后继
            Block* next = current->next;
            removeList(next);
            current->size += next->size + sizeof(Block);
        }
    }
    
    // 重新插入
    if (current != block) {
        insertList(current);
    }
}

int main() {
    const size_t POOL_SIZE = 1024 * 1024; // 1MB
    void* buffer = malloc(POOL_SIZE);
    
    MemoryPool pool(buffer, POOL_SIZE);
    
    // 分配内存
    void* ptr1 = pool.allocate(100);
    void* ptr2 = pool.allocate(200);
    
    // 释放内存
    pool.deallocate(ptr1);
    pool.deallocate(ptr2);
    
    free(buffer);
    return 0;
}