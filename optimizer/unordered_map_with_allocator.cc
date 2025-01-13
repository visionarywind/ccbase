// #include "allocator.h"

#include <array>
#include <unordered_map>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <map>

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
      std::cout << "free size : " << allocated[pointer] << std::endl;
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

template <typename T>
struct CustomAllocator : MemoryPool<T, 1024> {
  using value_type = T;

  CustomAllocator() = default;

  template <typename U>
  CustomAllocator(const CustomAllocator<U> &) {}

  T *allocate(std::size_t n) {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) throw std::bad_alloc();
    // return static_cast<T *>(malloc(n * sizeof(T)));
    return MemoryPool<T, 1024>::Borrow();
  }

  void deallocate(T *p, std::size_t) noexcept {
    // free(p);
    MemoryPool<T, 1024>::GiveBack(p);
  }
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
  std::thread t1 = std::thread([&]() {
    auto start = GetTick();
    for (size_t i = 0; i < 1; i++) {
      //   using MapAllocator = PoolAllocator<std::pair<const int, int>>;
      //   IntHash hash;
      //   std::unordered_map<int, int, IntHash, std::equal_to<const int>, MapAllocator> m(
      //     0, hash, std::equal_to<const int>(), MapAllocator(1024));
      //   CustomAllocator<std::pair<const int, int>> allocator;
      //   std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, CustomAllocator<std::pair<const int, int>>>
      //   m(
      //     0, std::hash<int>(), std::equal_to<int>(), allocator);

      std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, CustomAllocator<std::pair<const int, int>>> m;

      for (int i = 0; i < 10000; i++) {
        m[i] = (i);
      }
      for (int i = 0; i < 10000; i++) {
        m.erase(i);
      }
    }
    auto cost = GetTick() - start;
    printf("t1 cost : %lu ns.\n", cost);
  });

#ifdef T2
  std::thread t2 = std::thread([]() {
    auto start = GetTick();
    for (size_t i = 0; i < 100; i++) {
      std::unordered_map<int, std::string> m;
      for (int i = 0; i < 1000; i++) {
        m[i] = (i);
      }
      for (int i = 0; i < 1000; i++) {
        m.erase(i);
      }
    }
    auto cost = GetTick() - start;
    printf("t2 cost : %lu ns.\n", cost);
  });
#endif

  t1.join();
#ifdef T2
  t2.join();
#endif

  return 0;
}