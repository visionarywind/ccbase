#include <iostream>
#include <memory>
#include "call.h"

size_t totalAllocatedMemoryC = 0;
size_t totalFreedMemoryC = 0;

void *operator new(size_t size) {
  totalAllocatedMemoryC += size;
  std::cout << "Allocating " << size << " bytes. Total allocated so far: " << totalAllocatedMemoryC << " bytes."
            << std::endl;
  return malloc(size);
}

void operator delete(void *ptr, size_t size) {
  totalFreedMemoryC += size;
  std::cout << "Deallocating " << size << " bytes. Total freed so far: " << totalFreedMemoryC << " bytes." << std::endl;
  free(ptr);
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
}