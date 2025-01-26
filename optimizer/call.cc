#include <iostream>
#include <memory>
#include <map>
#include "call.h"

// 用于统计分配的总内存大小
std::size_t totalAllocated = 0;
std::size_t peakAllocated = 0;
// 用于统计当前已分配的内存大小
std::size_t currentAllocated = 0;
std::size_t allocCount = 0;
std::mutex memoryMutex;

template <typename T>
class CustomAllocator {
  // Custom allocator template
 public:
  using value_type = T;

  template <class U>
  struct rebind {
    typedef CustomAllocator<U> other;
  };

  CustomAllocator() = default;

  // Copy constructor
  template <typename U>
  CustomAllocator(const CustomAllocator<U> &) {}

  // Allocate memory
  T *allocate(std::size_t n) {
    if (n > std::size_t(-1) / sizeof(T)) {
      throw std::bad_alloc();
    }
    return static_cast<T *>(std::malloc(n * sizeof(T)));
  }

  // Deallocate memory
  void deallocate(T *p, std::size_t n) { std::free(p); }

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

typedef CustomAllocator<std::map<void *, size_t>::value_type> MapMemoryPoolAllocator;

std::map<void *, size_t, std::less<void *>, MapMemoryPoolAllocator> memoryMap;

// 全局重载operator new
void *operator new(std::size_t size) {
  std::lock_guard<std::mutex> lock(memoryMutex);
  void *ptr = std::malloc(size);
  if (!ptr) {
    throw std::bad_alloc();
  }
  allocCount++;
  totalAllocated += size;
  currentAllocated += size;
  peakAllocated = std::max(peakAllocated, currentAllocated);
  // if (allocCount % 10000 == 0) {
  std::cout << "peakAllocated : " << peakAllocated << ",  currentAllocated : " << currentAllocated << std::endl;
  // }
  memoryMap[ptr] = size;
  return ptr;
}

// 全局重载operator delete
void operator delete(void *ptr) noexcept {
  std::lock_guard<std::mutex> lock(memoryMutex);
  if (ptr) {
    if (memoryMap.find(ptr) == memoryMap.end()) {
      std::cout << "free failed : " << ptr << std::endl;
      std::free(ptr);
      return;
    }

    std::size_t size = memoryMap[ptr];
    currentAllocated -= size;
    memoryMap.erase(ptr);
    std::free(ptr);
  } else {
    std::cout << "free failed : " << ptr << std::endl;
  }
}

class C {
 public:
  C() {}
  ~C() {}
  void test() {
    for (int i = 0; i < 1; i++) {
      vec_.push_back(i);
    }
    vec_.clear();
  }
  std::vector<int> vec_;
};

void call() {
  auto c = new C();
  c->test();
  std::cout << "call" << std::endl;
  // delete c;
}

int main() {
  for (int i = 0; i < 1000000000; i++) {
    call();
  }
}