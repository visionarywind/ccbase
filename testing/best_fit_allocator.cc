#include <cstddef>
#include <cstdint>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>

// 内存块最小大小 (48字节)
constexpr size_t MIN_BLOCK_SIZE = 48;
// 内存对齐 (8字节)
constexpr size_t ALIGNMENT = 8;
// 保护标记 (0xABABABAB)
constexpr uint32_t GUARD_TAG = 0xABABABAB;

// 红黑树节点颜色
enum class Color { RED, BLACK };

// 内存块头尾元数据结构
struct BlockMeta {
    uint32_t size : 30;   // 块大小 (包括头尾)
    uint32_t alloc : 1;   // 分配标志
    uint32_t color : 1;   // 红黑树颜色 (仅空闲块有效)
};

// 空闲块数据结构 (使用用户数据区存储树节点)
struct FreeBlock {
    BlockMeta header;         // 块头部
    FreeBlock* left;          // 左子树
    FreeBlock* right;         // 右子树
    FreeBlock* parent;        // 父节点
    // 注意: 尾部元数据在块末尾
};

// 内存分配器类
class BestFitAllocator {
private:
    FreeBlock* root;          // 红黑树根节点
    void* heap_start;         // 堆起始位置
    void* heap_end;           // 堆结束位置
    size_t total_allocated;   // 总分配内存
    size_t total_freed;       // 总释放内存

    // 对齐计算
    size_t align_up(size_t size) {
        return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    }

    // 获取块总大小 (包括元数据)
    size_t get_total_size(size_t size) {
        size_t total = align_up(size + sizeof(BlockMeta) * 2);
        return total < MIN_BLOCK_SIZE ? MIN_BLOCK_SIZE : total;
    }

    // 获取块尾部指针
    BlockMeta* get_footer(FreeBlock* block) {
        return reinterpret_cast<BlockMeta*>(
            reinterpret_cast<char*>(block) + block->header.size - sizeof(BlockMeta));
    }

    // 获取下一个块
    FreeBlock* get_next_block(FreeBlock* block) {
        return reinterpret_cast<FreeBlock*>(
            reinterpret_cast<char*>(block) + block->header.size);
    }

    // 获取前一个块 (通过尾部)
    FreeBlock* get_prev_block(FreeBlock* block) {
        BlockMeta* prev_footer = reinterpret_cast<BlockMeta*>(
            reinterpret_cast<char*>(block) - sizeof(BlockMeta));
        return reinterpret_cast<FreeBlock*>(
            reinterpret_cast<char*>(prev_footer) - prev_footer->size + sizeof(BlockMeta));
    }

    // 初始化内存块
    void init_block(FreeBlock* block, size_t size, bool alloc) {
        block->header.size = size;
        block->header.alloc = alloc;
        block->header.color = alloc ? 0 : static_cast<uint32_t>(Color::RED);
        
        BlockMeta* footer = get_footer(block);
        footer->size = size;
        footer->alloc = alloc;
    }

    // 红黑树旋转辅助函数
    void rotate_left(FreeBlock* x) {
        FreeBlock* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        
        if (!x->parent) root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;
        
        y->left = x;
        x->parent = y;
    }

    void rotate_right(FreeBlock* x) {
        FreeBlock* y = x->left;
        x->left = y->right;
        if (y->right) y->right->parent = x;
        y->parent = x->parent;
        
        if (!x->parent) root = y;
        else if (x == x->parent->right) x->parent->right = y;
        else x->parent->left = y;
        
        y->right = x;
        x->parent = y;
    }

    // 红黑树插入修正
    void insert_fixup(FreeBlock* z) {
        while (z->parent && static_cast<Color>(z->parent->header.color) == Color::RED) {
            if (z->parent == z->parent->parent->left) {
                FreeBlock* y = z->parent->parent->right;
                if (y && static_cast<Color>(y->header.color) == Color::RED) {
                    z->parent->header.color = static_cast<uint32_t>(Color::BLACK);
                    y->header.color = static_cast<uint32_t>(Color::BLACK);
                    z->parent->parent->header.color = static_cast<uint32_t>(Color::RED);
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotate_left(z);
                    }
                    z->parent->header.color = static_cast<uint32_t>(Color::BLACK);
                    z->parent->parent->header.color = static_cast<uint32_t>(Color::RED);
                    rotate_right(z->parent->parent);
                }
            } else {
                FreeBlock* y = z->parent->parent->left;
                if (y && static_cast<Color>(y->header.color) == Color::RED) {
                    z->parent->header.color = static_cast<uint32_t>(Color::BLACK);
                    y->header.color = static_cast<uint32_t>(Color::BLACK);
                    z->parent->parent->header.color = static_cast<uint32_t>(Color::RED);
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotate_right(z);
                    }
                    z->parent->header.color = static_cast<uint32_t>(Color::BLACK);
                    z->parent->parent->header.color = static_cast<uint32_t>(Color::RED);
                    rotate_left(z->parent->parent);
                }
            }
        }
        root->header.color = static_cast<uint32_t>(Color::BLACK);
    }

    // 红黑树插入
    void rb_insert(FreeBlock* z) {
        FreeBlock* y = nullptr;
        FreeBlock* x = root;
        
        while (x) {
            y = x;
            if (z->header.size < x->header.size) x = x->left;
            else x = x->right;
        }
        
        z->parent = y;
        if (!y) root = z;
        else if (z->header.size < y->header.size) y->left = z;
        else y->right = z;
        
        z->left = z->right = nullptr;
        z->header.color = static_cast<uint32_t>(Color::RED);
        insert_fixup(z);
    }

    // 红黑树删除修正
    void delete_fixup(FreeBlock* x) {
        while (x != root && (!x || static_cast<Color>(x->header.color) == Color::BLACK)) {
            if (x == x->parent->left) {
                FreeBlock* w = x->parent->right;
                if (w && static_cast<Color>(w->header.color) == Color::RED) {
                    w->header.color = static_cast<uint32_t>(Color::BLACK);
                    x->parent->header.color = static_cast<uint32_t>(Color::RED);
                    rotate_left(x->parent);
                    w = x->parent->right;
                }
                if ((!w->left || static_cast<Color>(w->left->header.color) == Color::BLACK) &&
                    (!w->right || static_cast<Color>(w->right->header.color) == Color::BLACK)) {
                    w->header.color = static_cast<uint32_t>(Color::RED);
                    x = x->parent;
                } else {
                    if (!w->right || static_cast<Color>(w->right->header.color) == Color::BLACK) {
                        if (w->left) w->left->header.color = static_cast<uint32_t>(Color::BLACK);
                        w->header.color = static_cast<uint32_t>(Color::RED);
                        rotate_right(w);
                        w = x->parent->right;
                    }
                    w->header.color = static_cast<Color>(x->parent->header.color);
                    x->parent->header.color = static_cast<uint32_t>(Color::BLACK);
                    if (w->right) w->right->header.color = static_cast<uint32_t>(Color::BLACK);
                    rotate_left(x->parent);
                    x = root;
                }
            } else {
                FreeBlock* w = x->parent->left;
                if (w && static_cast<Color>(w->header.color) == Color::RED) {
                    w->header.color = static_cast<uint32_t>(Color::BLACK);
                    x->parent->header.color = static_cast<uint32_t>(Color::RED);
                    rotate_right(x->parent);
                    w = x->parent->left;
                }
                if ((!w->right || static_cast<Color>(w->right->header.color) == Color::BLACK) &&
                    (!w->left || static_cast<Color>(w->left->header.color) == Color::BLACK)) {
                    w->header.color = static_cast<uint32_t>(Color::RED);
                    x = x->parent;
                } else {
                    if (!w->left || static_cast<Color>(w->left->header.color) == Color::BLACK) {
                        if (w->right) w->right->header.color = static_cast<uint32_t>(Color::BLACK);
                        w->header.color = static_cast<uint32_t>(Color::RED);
                        rotate_left(w);
                        w = x->parent->left;
                    }
                    w->header.color = static_cast<Color>(x->parent->header.color);
                    x->parent->header.color = static_cast<uint32_t>(Color::BLACK);
                    if (w->left) w->left->header.color = static_cast<uint32_t>(Color::BLACK);
                    rotate_right(x->parent);
                    x = root;
                }
            }
        }
        if (x) x->header.color = static_cast<uint32_t>(Color::BLACK);
    }

    // 红黑树删除
    void rb_delete(FreeBlock* z) {
        if (!z) return;
        
        FreeBlock* y = z;
        FreeBlock* x;
        Color y_original_color = static_cast<Color>(y->header.color);
        
        if (!z->left) {
            x = z->right;
            if (!z->parent) root = z->right;
            else if (z == z->parent->left) z->parent->left = z->right;
            else z->parent->right = z->right;
            if (z->right) z->right->parent = z->parent;
        } else if (!z->right) {
            x = z->left;
            if (!z->parent) root = z->left;
            else if (z == z->parent->left) z->parent->left = z->left;
            else z->parent->right = z->left;
            if (z->left) z->left->parent = z->parent;
        } else {
            y = z->right;
            while (y->left) y = y->left;
            y_original_color = static_cast<Color>(y->header.color);
            x = y->right;
            
            if (y->parent == z) {
                if (x) x->parent = y;
            } else {
                if (x) x->parent = y->parent;
                y->parent->left = x;
                y->right = z->right;
                z->right->parent = y;
            }
            
            if (!z->parent) root = y;
            else if (z == z->parent->left) z->parent->left = y;
            else z->parent->right = y;
            
            y->parent = z->parent;
            y->left = z->left;
            z->left->parent = y;
            y->header.color = z->header.color;
        }
        
        if (y_original_color == Color::BLACK && x)
            delete_fixup(x);
    }

    // 搜索最佳匹配 (最小的大于等于所需大小的块)
    FreeBlock* search_best_fit(size_t size) {
        FreeBlock* current = root;
        FreeBlock* best = nullptr;
        
        while (current) {
            if (current->header.size >= size) {
                best = current;
                current = current->left; // 尝试找更小的满足块
            } else {
                current = current->right;
            }
        }
        return best;
    }

    // 扩展堆空间
    FreeBlock* expand_heap(size_t size) {
        // 计算需要申请的大小 (至少1页)
        size_t page_size = sysconf(_SC_PAGESIZE);
        size_t request_size = (size + page_size - 1) / page_size * page_size;
        
        void* block = sbrk(request_size);
        if (block == reinterpret_cast<void*>(-1)) return nullptr;
        
        // 初始化新块
        FreeBlock* new_block = reinterpret_cast<FreeBlock*>(block);
        init_block(new_block, request_size, false);
        
        // 更新堆边界
        if (!heap_start) heap_start = block;
        heap_end = reinterpret_cast<char*>(block) + request_size;
        
        total_allocated += request_size;
        return new_block;
    }

    // 分割内存块
    void* split_block(FreeBlock* block, size_t size) {
        size_t remaining = block->header.size - size;
        
        // 如果剩余空间足够新块，则分割
        if (remaining >= MIN_BLOCK_SIZE) {
            // 创建新空闲块
            FreeBlock* new_block = reinterpret_cast<FreeBlock*>(
                reinterpret_cast<char*>(block) + size);
            init_block(new_block, remaining, false);
            
            // 插入红黑树
            rb_insert(new_block);
            
            // 调整原块大小
            block->header.size = size;
            get_footer(block)->size = size;
        }
        
        // 标记为已分配
        block->header.alloc = true;
        get_footer(block)->alloc = true;
        
        // 返回用户数据区指针 (在头部之后)
        return reinterpret_cast<void*>(reinterpret_cast<char*>(block) + sizeof(BlockMeta));
    }

    // 合并相邻空闲块
    FreeBlock* coalesce_blocks(FreeBlock* block) {
        // 检查前一个块
        if (block != heap_start) {
            FreeBlock* prev = get_prev_block(block);
            if (prev && !prev->header.alloc) {
                // 从树中删除前块
                rb_delete(prev);
                
                // 合并块
                prev->header.size += block->header.size;
                get_footer(prev)->size = prev->header.size;
                
                block = prev;
            }
        }
        
        // 检查后一个块
        if (reinterpret_cast<char*>(block) + block->header.size < reinterpret_cast<char*>(heap_end)) {
            FreeBlock* next = get_next_block(block);
            if (next && !next->header.alloc) {
                // 从树中删除后块
                rb_delete(next);
                
                // 合并块
                block->header.size += next->header.size;
                get_footer(block)->size = block->header.size;
            }
        }
        
        return block;
    }

public:
    BestFitAllocator() 
        : root(nullptr), heap_start(nullptr), heap_end(nullptr), 
          total_allocated(0), total_freed(0) {
        // 初始申请4页内存
        size_t page_size = sysconf(_SC_PAGESIZE);
        heap_start = sbrk(4 * page_size);
        heap_end = reinterpret_cast<char*>(heap_start) + 4 * page_size;
        
        // 初始化保护块 (永不分配)
        FreeBlock* guard = reinterpret_cast<FreeBlock*>(heap_start);
        guard->header.size = GUARD_TAG;
        guard->header.alloc = true;
        
        // 初始化主空闲块
        FreeBlock* main_block = reinterpret_cast<FreeBlock*>(
            reinterpret_cast<char*>(heap_start) + sizeof(BlockMeta));
        size_t main_size = 4 * page_size - 2 * sizeof(BlockMeta);
        init_block(main_block, main_size, false);
        
        // 插入红黑树
        root = main_block;
        main_block->left = main_block->right = main_block->parent = nullptr;
        main_block->header.color = static_cast<uint32_t>(Color::BLACK);
        
        total_allocated = 4 * page_size;
    }

    ~BestFitAllocator() {
        // 重置堆指针
        brk(heap_start);
    }

    // 内存分配函数
    void* allocate(size_t size) {
        if (size == 0) return nullptr;
        
        // 计算所需总大小
        size_t total_size = get_total_size(size);
        
        // 搜索最佳匹配
        FreeBlock* block = search_best_fit(total_size);
        
        // 没有合适块则扩展堆
        if (!block) {
            block = expand_heap(total_size);
            if (!block) return nullptr;
            rb_insert(block);
        }
        
        // 从树中删除找到的块
        rb_delete(block);
        
        // 分割并返回分配块
        return split_block(block, total_size);
    }

    // 内存释放函数
    void deallocate(void* ptr) {
        if (!ptr) return;
        
        // 获取块起始位置 (用户指针前移得到块头)
        FreeBlock* block = reinterpret_cast<FreeBlock*>(
            reinterpret_cast<char*>(ptr) - sizeof(BlockMeta));
        
        // 验证块标记
        if (block->header.size == GUARD_TAG) return;
        if (get_footer(block)->size != block->header.size) return;
        
        // 标记为空闲
        block->header.alloc = false;
        get_footer(block)->alloc = false;
        
        // 合并相邻空闲块
        block = coalesce_blocks(block);
        
        // 插入红黑树
        block->left = block->right = block->parent = nullptr;
        rb_insert(block);
        
        total_freed += block->header.size;
    }

    // 诊断函数
    void print_stats() const {
        std::cout << "Memory Allocator Stats:\n";
        std::cout << "  Total Allocated: " << total_allocated << " bytes\n";
        std::cout << "  Total Freed: " << total_freed << " bytes\n";
        std::cout << "  Current Usage: " << (total_allocated - total_freed) << " bytes\n";
    }
};

// 全局分配器实例
static BestFitAllocator allocator;

// 重载的malloc和free函数
extern "C" void* bf_malloc(size_t size) {
    return allocator.allocate(size);
}

extern "C" void bf_free(void* ptr) {
    allocator.deallocate(ptr);
}

// 测试函数
void test_allocator() {
    // 基本分配/释放测试
    void* p1 = bf_malloc(100);
    void* p2 = bf_malloc(200);
    void* p3 = bf_malloc(300);
    
    std::cout << "Allocated p1: " << p1 << "\n";
    std::cout << "Allocated p2: " << p2 << "\n";
    std::cout << "Allocated p3: " << p3 << "\n";
    
    bf_free(p2);
    bf_free(p1);
    bf_free(p3);
    
    // 最佳匹配测试
    void* small = bf_malloc(16);
    void* medium = bf_malloc(128);
    void* large = bf_malloc(1024);
    
    std::cout << "Allocated small: " << small << "\n";
    std::cout << "Allocated medium: " << medium << "\n";
    std::cout << "Allocated large: " << large << "\n";
    
    bf_free(medium);
    
    // 应重用medium块
    void* reuse = bf_malloc(120);
    std::cout << "Reused block: " << reuse << " (should be same as medium: " << medium << ")\n";
    
    bf_free(small);
    bf_free(large);
    bf_free(reuse);
    
    // 合并测试
    void* a = bf_malloc(64);
    void* b = bf_malloc(64);
    void* c = bf_malloc(64);
    
    std::cout << "Allocated a: " << a << "\n";
    std::cout << "Allocated b: " << b << "\n";
    std::cout << "Allocated c: " << c << "\n";
    
    bf_free(b);
    bf_free(a); // 应合并a和b
    
    bf_free(c);
    
    allocator.print_stats();
}

int main() {
    test_allocator();
    return 0;
}