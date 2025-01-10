#include <memory>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <vector>
#include <cstddef>
#include <cassert>

template <class T, std::size_t growSize = 1024>
class MemoryPool {
  struct Block {
    Block *next;
  };

  class Buffer {
    static const std::size_t blockSize = sizeof(T) > sizeof(Block) ? sizeof(T) : sizeof(Block);
    uint8_t data[blockSize * growSize];

   public:
    Buffer *const next;

    Buffer(Buffer *next) : next(next) {}

    T *getBlock(std::size_t index) { return reinterpret_cast<T *>(&data[blockSize * index]); }
  };

  Block *firstFreeBlock = nullptr;
  Buffer *firstBuffer = nullptr;
  std::size_t bufferedBlocks = growSize;

 public:
  MemoryPool() = default;
  MemoryPool(MemoryPool &&memoryPool) = delete;
  MemoryPool(const MemoryPool &memoryPool) = delete;
  MemoryPool operator=(MemoryPool &&memoryPool) = delete;
  MemoryPool operator=(const MemoryPool &memoryPool) = delete;

  ~MemoryPool() {
    while (firstBuffer) {
      Buffer *buffer = firstBuffer;
      firstBuffer = buffer->next;
      delete buffer;
    }
  }

  T *allocate() {
    // std::cout << "allocate : " << sizeof(T) << std::endl;
    if (firstFreeBlock) {
      Block *block = firstFreeBlock;
      firstFreeBlock = block->next;
      return reinterpret_cast<T *>(block);
    }

    if (bufferedBlocks >= growSize) {
      firstBuffer = new Buffer(firstBuffer);
      // std::cout << "new buffer : " << firstBuffer << std::endl;
      bufferedBlocks = 0;
    }

    auto ret = firstBuffer->getBlock(bufferedBlocks++);
    // std::cout << "return : " << ret << std::endl;
    return ret;
  }

  void deallocate(T *pointer) {
    // std::cout << "free : " << pointer << ", " << sizeof(T) << std::endl;
    Block *block = reinterpret_cast<Block *>(pointer);
    block->next = firstFreeBlock;
    firstFreeBlock = block;
  }
};

// Custom allocator template
template <typename T, std::size_t growSize = 1024>
class CustomAllocator : private MemoryPool<T, growSize> {
  // Custom allocator template
 public:
  using value_type = T;

  template <class U>
  struct rebind {
    typedef CustomAllocator<U, growSize> other;
  };

  CustomAllocator() = default;

  // Copy constructor
  template <typename U>
  CustomAllocator(const CustomAllocator<U> &) {}

  // Allocate memory
  T *allocate(std::size_t n) {
    // std::cout << "Allocating " << n << " element(s) of size " << sizeof(T) << '\n';
    if (n > std::size_t(-1) / sizeof(T)) {
      throw std::bad_alloc();
    }
    return static_cast<T *>(::operator new(n * sizeof(T)));
    // return MemoryPool<T, growSize>::allocate();
  }

  // Deallocate memory
  void deallocate(T *p, std::size_t n) {
    // std::cout << "Deallocating " << n << " element(s) of size " << sizeof(T) << '\n';
    ::operator delete(p);
    // MemoryPool<T, growSize>::deallocate(p);
  }

  // Equality operators
  template <typename U>
  bool operator==(const CustomAllocator<U> &) const {
    return true;
  }

  template <typename U>
  bool operator!=(const CustomAllocator<U> &) const {
    return false;
  }
};

template <class T, std::size_t growSize = 1024>
class Allocator : private MemoryPool<T, growSize> {
#if defined(_WIN32) && defined(ENABLE_OLD_WIN32_SUPPORT)
  Allocator *copyAllocator = nullptr;
  std::allocator<T> *rebindAllocator = nullptr;
#endif

 public:
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef T &reference;
  typedef const T &const_reference;
  typedef T value_type;

  template <class U>
  struct rebind {
    typedef Allocator<U, growSize> other;
  };

#if defined(_WIN32) && defined(ENABLE_OLD_WIN32_SUPPORT)
  Allocator() = default;

  Allocator(Allocator &allocator) : copyAllocator(&allocator) {}

  template <class U>
  Allocator(const Allocator<U, growSize> &other) {
    if (!std::is_same<T, U>::value) rebindAllocator = new std::allocator<T>();
  }

  ~Allocator() { delete rebindAllocator; }
#endif

  pointer allocate(size_type n, const void *hint = 0) {
#if defined(_WIN32) && defined(ENABLE_OLD_WIN32_SUPPORT)
    if (copyAllocator) return copyAllocator->allocate(n, hint);

    if (rebindAllocator) return rebindAllocator->allocate(n, hint);
#endif

    if (n != 1 || hint) throw std::bad_alloc();

    return MemoryPool<T, growSize>::allocate();
  }

  void deallocate(pointer p, size_type n) {
#if defined(_WIN32) && defined(ENABLE_OLD_WIN32_SUPPORT)
    if (copyAllocator) {
      copyAllocator->deallocate(p, n);
      return;
    }

    if (rebindAllocator) {
      rebindAllocator->deallocate(p, n);
      return;
    }
#endif

    MemoryPool<T, growSize>::deallocate(p);
  }

  void construct(pointer p, const_reference val) { new (p) T(val); }

  void destroy(pointer p) { p->~T(); }
};

uint64_t GetTick() {
  auto &&ts = std::chrono::system_clock::now();
  int64_t system_t = std::chrono::duration_cast<std::chrono::nanoseconds>(ts.time_since_epoch()).count();
  return static_cast<uint64_t>(system_t);
}

template <typename T>
class PoolAllocator {
 public:
  using value_type = T;

  // Constructor
  PoolAllocator(size_t poolSize = 1024) : poolSize_(poolSize), pool_(nullptr), freeList_(nullptr) { allocatePool(); }

  // Destructor
  ~PoolAllocator() { ::operator delete(pool_); }

  // Copy constructor (required for allocator)
  template <typename U>
  PoolAllocator(const PoolAllocator<U> &other)
      : poolSize_(other.poolSize_), pool_(other.pool_), freeList_(other.freeList_) {}

  // Allocate memory for `n` objects
  T *allocate(std::size_t n) {
    if (n != 1) {
      throw std::bad_alloc();
    }

    if (!freeList_) {
      throw std::bad_alloc();
    }

    // Take a chunk from the free list
    T *ptr = freeList_;
    freeList_ = freeList_->next;
    return reinterpret_cast<T *>(ptr);
  }

  // Deallocate memory
  void deallocate(T *p, std::size_t n) {
    if (!p || n != 1) {
      return;
    }

    // Return the chunk to the free list
    Node *node = reinterpret_cast<Node *>(p);
    node->next = freeList_;
    freeList_ = node;
  }

  template <typename U>
  struct rebind {
    using other = PoolAllocator<U>;
  };

  template <typename U>
  bool operator==(const PoolAllocator<U> &) const {
    return true;
  }

  template <typename U>
  bool operator!=(const PoolAllocator<U> &) const {
    return false;
  }

 public:
  struct Node {
    Node *next;
  };

  size_t poolSize_;  // Number of elements in the pool
  Node *pool_;       // Pool of memory
  Node *freeList_;   // Free list of nodes

  void allocatePool() {
    pool_ = static_cast<Node *>(::operator new(poolSize_ * sizeof(Node)));

    // Link all nodes in the free list
    freeList_ = pool_;
    for (size_t i = 0; i < poolSize_ - 1; ++i) {
      pool_[i].next = &pool_[i + 1];
    }
    pool_[poolSize_ - 1].next = nullptr;
  }
};
