// #include "allocator.h"
// #include <gperftools/tcmalloc.h>
// #include <gperftools/tcmalloc.h>
#include <array>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <map>
// #include <jemalloc/jemalloc.h>
using namespace std::chrono_literals;

struct IntHash {
  template <class T1, class T2>
  std::size_t operator()(std::pair<T1, T2> const &v) const {
    return std::hash<T1>()(v);
  }
};

uint64_t GetTick() {
  auto &&ts = std::chrono::system_clock::now();
  int64_t system_t = std::chrono::duration_cast<std::chrono::nanoseconds>(ts.time_since_epoch()).count();
  return static_cast<uint64_t>(system_t);
}

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

  T *Borrow() {
    std::cout << "allocate : " << sizeof(T) << std::endl;
    if (firstFreeBlock) {
      Block *block = firstFreeBlock;
      firstFreeBlock = block->next;
      allocated.emplace(block, sizeof(T));

      std::cout << "allocate : " << sizeof(T) << ", ret : " << block << std::endl;
      return reinterpret_cast<T *>(block);
    }

    if (bufferedBlocks >= growSize) {
      firstBuffer = new Buffer(firstBuffer);
      // std::cout << "new buffer : " << firstBuffer << std::endl;
      bufferedBlocks = 0;
    }

    auto ret = firstBuffer->getBlock(bufferedBlocks++);
    // std::cout << "return : " << ret << std::endl;
    allocated.emplace(ret, sizeof(T));
    std::cout << "allocate : " << sizeof(T) << ", ret : " << ret << std::endl;
    return ret;
  }

  void GiveBack(T *pointer) {
    if (allocated.find(pointer) != allocated.end()) {
      // std::cout << "free size : " << allocated[pointer] << std::endl;
      allocated.erase(pointer);
    } else {
      std::cout << "free error : " << pointer << std::endl;
    }

    // std::cout << "free : " << pointer << ", " << sizeof(T) << std::endl;
    Block *block = reinterpret_cast<Block *>(pointer);
    block->next = firstFreeBlock;
    firstFreeBlock = block;
  }

  std::map<void *, size_t> allocated;
};

template <class T, std::size_t growSize = 1024>
struct Pooled {
  std::map<size_t, MemoryPool<T, growSize> *> pools;
  std::map<void *, MemoryPool<T, growSize> *> allocated;
};

template <class T>
class BufferPool {
  struct Block {
    Block *next;
  };

  static const size_t growSize = 1024;
  class Buffer {
    uint8_t *buffer_;

   public:
    Buffer *const next;
    size_t block_size_;

    Buffer(Buffer *next, size_t block_size) : next(next), block_size_(block_size) {
      buffer_ = new uint8_t[growSize * block_size_];
    }
    ~Buffer() { delete[] buffer_; }

    T *getBlock(std::size_t index) { return reinterpret_cast<T *>(buffer_ + index * growSize); }
  };

  const size_t block_size_;
  Block *firstFreeBlock = nullptr;
  Buffer *firstBuffer = nullptr;
  std::size_t bufferedBlocks = growSize;

 public:
  BufferPool(const size_t block_size)
      : block_size_(block_size) {
          // std::cout << "new Buffer pool[" << this << "], block size : " << block_size_ << ", sizeof(T) : " <<
          // sizeof(T)
          //           << std::endl;
        };
  BufferPool(BufferPool &&memoryPool) = delete;
  BufferPool(const BufferPool &memoryPool) = delete;
  BufferPool operator=(BufferPool &&memoryPool) = delete;
  BufferPool operator=(const BufferPool &memoryPool) = delete;

  ~BufferPool() {
    while (firstBuffer) {
      Buffer *buffer = firstBuffer;
      firstBuffer = buffer->next;
      delete buffer;
    }
  }

  T *Borrow() {
    // std::cout << "BufferPool[" << this << "] - allocate : " << typeid(T).name() << std::endl;
    if (firstFreeBlock) {
      Block *block = firstFreeBlock;
      firstFreeBlock = block->next;

      // std::cout << "BufferPool[" << this << "] - allocate : " << sizeof(T) << ", ret : " << block << std::endl;
      return reinterpret_cast<T *>(block);
    }

    if (bufferedBlocks >= growSize) {
      firstBuffer = new Buffer(firstBuffer, block_size_);
      bufferedBlocks = 0;
    }

    auto ret = firstBuffer->getBlock(bufferedBlocks++);
    // std::cout << "BufferPool[" << this << "] - allocate : " << sizeof(T) << ", ret : " << ret << std::endl;
    return ret;
  }

  void GiveBack(T *pointer) {
    // std::cout << "BufferPool[" << this << "] - GiveBack : " << pointer << std::endl;
    Block *block = reinterpret_cast<Block *>(pointer);
    block->next = firstFreeBlock;
    firstFreeBlock = block;
  }
};

template <typename T>
struct CustomAllocator {
  using value_type = T;

  CustomAllocator() = default;

  template <typename U>
  CustomAllocator(const CustomAllocator<U> &) {}

  T *allocate(std::size_t n) {
    std::cout << "CustomAllocator - allocate : " << n << std::endl;
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) throw std::bad_alloc();
    // return static_cast<T *>(malloc(n * sizeof(T)));
    // return pool_.Borrow();
    auto it = pool_.find(n * sizeof(T));
    if (it == pool_.end()) {
      pool_[n * sizeof(T)] = new BufferPool<T>(n * sizeof(T));
      auto ret = pool_[n * sizeof(T)]->Borrow();
      allocated[ret] = pool_[n * sizeof(T)];
      return ret;
    }
    auto ret = it->second->Borrow();
    allocated[ret] = it->second;
    return ret;
  }

  void deallocate(T *p, std::size_t) noexcept {
    // std::cout << "CustomAllocator - deallocate : " << p << std::endl;
    auto it = allocated.find(p);
    if (it != allocated.end()) {
      it->second->GiveBack(p);
      allocated.erase(it);
    } else {
      std::cout << "deallocate error : " << p << std::endl;
    }
  }

  // MemoryPool<T, 1024 * 1024 * 100> pool_;
  std::map<size_t, BufferPool<T> *> pool_;
  std::unordered_map<void *, BufferPool<T> *> allocated;
};

// 比较操作符
template <typename T, typename U>
bool operator==(const CustomAllocator<T> &, const CustomAllocator<U> &) {
  return true;
}

template <typename T, typename U>
bool operator!=(const CustomAllocator<T> &, const CustomAllocator<U> &) {
  return false;
}

// todo, not passthrough current allocator
int main() {
#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

  // je_mallctl("arena." STRINGIFY(MALLCTL_ARENAS_ALL) ".decay", NULL, NULL, NULL, 0);

  // 关闭内存剖析
  //   if (je_mallctl("prof.active", NULL, NULL, (void *)true, sizeof(bool)) != 0) {
  //     perror("");
  //     return 1;
  //   }

  std::thread t1 = std::thread([&]() {
    auto start = GetTick();
    for (size_t i = 0; i < 100; i++) {
      //   using MapAllocator = PoolAllocator<std::pair<const int, int>>;
      //   IntHash hash;
      //   std::unordered_map<int, int, IntHash, std::equal_to<const int>, MapAllocator> m(
      //     0, hash, std::equal_to<const int>(), MapAllocator(1024));
      //   CustomAllocator<std::pair<const int, int>> allocator;
      //   std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, CustomAllocator<std::pair<const int, int>>>
      //   m(
      //     0, std::hash<int>(), std::equal_to<int>(), allocator);

      std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, CustomAllocator<std::pair<const int, int>>> m;

      // std::cout << "Hello waiter\n" << std::flush;
      // std::this_thread::sleep_for(2ms);

      int cycle_size = 10;
      for (int i = 0; i < cycle_size; i++) {
        m[i] = (i);
      }
      for (int i = 0; i < cycle_size; i++) {
        m.erase(i);
      }
    }
    auto cost = GetTick() - start;
    std::cout << "t1 cost : " << cost << " ns.\n";
  });

#define T2
#ifdef T2
  std::thread t2 = std::thread([]() {
    auto start = GetTick();
    for (size_t i = 0; i < 100; i++) {
      std::unordered_map<int, std::string> m;

      int cycle_size = 10;
      for (int i = 0; i < cycle_size; i++) {
        m[i] = (i);
      }
      for (int i = 0; i < cycle_size; i++) {
        m.erase(i);
      }
    }
    auto cost = GetTick() - start;
    std::cout << "t2 cost : " << cost << " ns.\n";
  });
#endif

  t1.join();
#ifdef T2
  t2.join();
#endif

  //   malloc_dump("1.dump");

  return 0;
}
