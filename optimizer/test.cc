#include <iostream>
#include <unordered_map>
#include <memory>
#include <cstddef>
#include <vector>

template <typename T>
class MemoryPoolAllocator {
 public:
  using value_type = T;

  explicit MemoryPoolAllocator(std::size_t poolSize = 1024) : poolSize_(poolSize), pool_(nullptr), freeList_(nullptr) {
    allocatePool();
  }

  ~MemoryPoolAllocator() { ::operator delete(pool_); }

  template <typename U>
  MemoryPoolAllocator(const MemoryPoolAllocator<U> &other)
      : poolSize_(other.poolSize_),
        pool_(reinterpret_cast<Node *>(other.pool_)),
        freeList_(reinterpret_cast<Node *>(other.freeList_)) {}

  T *allocate(std::size_t n) {
    if (n > 1 || freeList_ == nullptr) {
      throw std::bad_alloc();
    }
    Node *node = freeList_;
    freeList_ = freeList_->next;
    return reinterpret_cast<T *>(node);
  }

  void deallocate(T *p, std::size_t n) {
    if (!p || n > 1) return;
    Node *node = reinterpret_cast<Node *>(p);
    node->next = freeList_;
    freeList_ = node;
  }

  template <typename U>
  struct rebind {
    using other = MemoryPoolAllocator<U>;
  };

  template <typename U>
  bool operator==(const MemoryPoolAllocator<U> &) const {
    return true;
  }

  template <typename U>
  bool operator!=(const MemoryPoolAllocator<U> &) const {
    return false;
  }

 public:
  struct Node {
    Node *next;
  };

  std::size_t poolSize_;  // Number of elements in the pool
  Node *pool_;            // Pointer to the memory pool
  Node *freeList_;        // Pointer to the free list

  void allocatePool() {
    pool_ = static_cast<Node *>(::operator new(poolSize_ * sizeof(Node)));
    freeList_ = pool_;
    for (std::size_t i = 0; i < poolSize_ - 1; ++i) {
      pool_[i].next = &pool_[i + 1];
    }
    pool_[poolSize_ - 1].next = nullptr;
  }
};
struct int_hash {
  std::size_t operator()(const int param) const { return std::hash<size_t>{}(param); }
};

int main() {
  using CustomAllocator = MemoryPoolAllocator<std::pair<const int, int>>;
  int_hash hasher;
  CustomAllocator allocator(1024);
  // Create an unordered_map using the memory pool allocator
  std::unordered_map<const int, int, int_hash, std::equal_to<const int>, CustomAllocator> customMap(
    0, hasher, std::equal_to<const int>(), allocator);

  // Insert some values
  const int value = 4;
  customMap[1] = value;
  customMap[2] = value;
  customMap[3] = value;

  // Display the values
  for (const auto &[key, value] : customMap) {
    std::cout << key << ": " << value << '\n';
  }

  return 0;
}