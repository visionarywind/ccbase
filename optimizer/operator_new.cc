#include <cstdlib>
#include <iostream>
#include <vector>

// #include "call.h"

size_t totalAllocatedMemory = 0;
size_t totalFreedMemory = 0;

void *operator new(size_t size) {
  totalAllocatedMemory += size;
  std::cout << "Allocating " << size << " bytes. Total allocated so far: " << totalAllocatedMemory << " bytes."
            << std::endl;
  return malloc(size);
}

void operator delete(void *ptr, size_t size) {
  totalFreedMemory += size;
  std::cout << "Deallocating " << size << " bytes. Total freed so far: " << totalFreedMemory << " bytes." << std::endl;
  free(ptr);
}

class MyClass {
 public:
  MyClass() {}
  ~MyClass() {}
  void Process() {
    for (int i = 0; i < 1; i++) {
      vec_.push_back(i);
    }
    vec_.clear();
  }
  std::vector<int> vec_;
};

namespace A {
/*
size_t totalAllocatedMemory = 0;
size_t totalFreedMemory = 0;

void *operator new(size_t size) {
totalAllocatedMemory += size;
std::cout << "Allocating " << size << " bytes. Total allocated so far: " << totalAllocatedMemory << " bytes."
        << std::endl;
return malloc(size);
}

void operator delete(void *ptr, size_t size) {
totalFreedMemory += size;
std::cout << "Deallocating " << size << " bytes. Total freed so far: " << totalFreedMemory << " bytes." << std::endl;
free(ptr);
}*/

class MyClass {
 public:
  MyClass() {}
  ~MyClass() {}
  void Process() {
    for (int i = 0; i < 1; i++) {
      vec_.push_back(i);
    }
    vec_.clear();
  }
  std::vector<int> vec_;
};
}  // namespace A

namespace B {
/*
size_t totalAllocatedMemory = 0;
size_t totalFreedMemory = 0;

void *operator new(size_t size) {
totalAllocatedMemory += size;
std::cout << "Allocating " << size << " bytes. Total allocated so far: " << totalAllocatedMemory << " bytes."
        << std::endl;
return malloc(size);
}

void operator delete(void *ptr, size_t size) {
totalFreedMemory += size;
std::cout << "Deallocating " << size << " bytes. Total freed so far: " << totalFreedMemory << " bytes." << std::endl;
free(ptr);
}
*/

class MyClass {
 public:
  MyClass() {}
  ~MyClass() {}
  void Process() {
    for (int i = 0; i < 1; i++) {
      vec_.push_back(i);
    }
    vec_.clear();
  }
  std::vector<int> vec_;
};
}  // namespace B

void test() {
  MyClass *obj1 = new MyClass();
  obj1->Process();
  delete obj1;

  MyClass *obj2 = new MyClass();
  obj2->Process();
  delete obj2;

  auto obj = std::make_shared<MyClass>();
  obj->Process();

  std::cout << "Total allocated memory: " << totalAllocatedMemory << " bytes." << std::endl;
  std::cout << "Total freed memory: " << totalFreedMemory << " bytes." << std::endl;
}

void testA() {
  A::MyClass *obj1 = new A::MyClass();
  obj1->Process();
  delete obj1;

  A::MyClass *obj2 = new A::MyClass();
  obj2->Process();
  delete obj2;

  auto obj = std::make_shared<A::MyClass>();
  obj->Process();

  std::cout << "Total allocated memory: " << totalAllocatedMemory << " bytes." << std::endl;
  std::cout << "Total freed memory: " << totalFreedMemory << " bytes." << std::endl;
}

void testB() {
  B::MyClass *obj1 = new B::MyClass();
  obj1->Process();
  delete obj1;

  B::MyClass *obj2 = new B::MyClass();
  obj2->Process();
  delete obj2;

  auto obj = std::make_shared<B::MyClass>();
  obj->Process();

  std::cout << "Total allocated memory: " << totalAllocatedMemory << " bytes." << std::endl;
  std::cout << "Total freed memory: " << totalFreedMemory << " bytes." << std::endl;
}

int main() {
  // call();
  test();
  testA();
  testB();
  return 0;
}