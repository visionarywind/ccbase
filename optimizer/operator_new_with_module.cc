#include <iostream>
#include <cstdlib>
#include <memory>
#include <unordered_map>

size_t totalAllocatedMemory = 0;
size_t totalFreedMemory = 0;

void *operator new(size_t size) {
  totalAllocatedMemory += size;
  std::cout << "Allocating1 " << size << " bytes. Total allocated so far: " << totalAllocatedMemory << " bytes."
            << std::endl;
  return malloc(size);
}

void operator delete(void *ptr, size_t size) {
  totalFreedMemory += size;
  std::cout << "Deallocating1 " << size << " bytes. Total freed so far: " << totalFreedMemory << " bytes." << std::endl;
  free(ptr);
}

struct AllocationInfo {
  size_t allocatedMemory;
  size_t freedMemory;
};

std::unordered_map<std::string, AllocationInfo> allocationMap;

void *operator new(size_t size, const char *module) {
  void *ptr = malloc(size);
  allocationMap[module].allocatedMemory += size;
  std::cout << "Allocating " << size << " bytes in module " << module
            << ". Total allocated so far: " << allocationMap[module].allocatedMemory << " bytes." << std::endl;
  return ptr;
}

void operator delete(void *ptr, size_t size, const char *module) {
  allocationMap[module].freedMemory += size;
  std::cout << "Deallocating " << size << " bytes in module " << module
            << ". Total freed so far: " << allocationMap[module].freedMemory << " bytes." << std::endl;
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
int main() {
  MyClass *obj1 = new ("Module1") MyClass();
  obj1->Process();
  delete obj1;

  MyClass *obj2 = new ("Module2") MyClass();
  obj2->Process();
  delete obj2;

  auto obj = std::make_shared<MyClass>();
  obj->Process();

  for (const auto &pair : allocationMap) {
    std::cout << "Module: " << pair.first << ", Total allocated memory: " << pair.second.allocatedMemory
              << " bytes, Total freed memory: " << pair.second.freedMemory << " bytes." << std::endl;
  }

  return 0;
}