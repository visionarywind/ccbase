#include <iostream>
#include <memory>
#include <unordered_map>

struct AllocationInfo {
  size_t allocatedMemory;
};

std::unordered_map<void *, AllocationInfo> allocationMap;

template <typename T, typename... Args>
std::shared_ptr<T> myMakeShared(Args &&...args) {
  size_t size = sizeof(T);
  void *ptr = ::operator new(size);
  try {
    T *obj = new (ptr) T(std::forward<Args>(args)...);
    allocationMap[ptr].allocatedMemory = size;
    return std::shared_ptr<T>(obj, [ptr](T *p) {
      p->~T();
      ::operator delete(ptr);
      allocationMap.erase(ptr);
    });
  } catch (...) {
    ::operator delete(ptr);
    throw;
  }
}

class MyClass {
 public:
  MyClass() { std::cout << "MyClass constructor called." << std::endl; }
  ~MyClass() { std::cout << "MyClass destructor called." << std::endl; }
  void Process() {
    for (int i = 0; i < 1; i++) {
      vec_.push_back(i);
    }
    vec_.clear();
  }
  std::vector<int> vec_;
};

int main() {
  auto ptr1 = myMakeShared<MyClass>();
  auto ptr2 = myMakeShared<MyClass>();
  ptr1->Process();
  ptr2->Process();

  for (const auto &pair : allocationMap) {
    std::cout << "Allocated memory for pointer: " << pair.first << " is " << pair.second.allocatedMemory << " bytes."
              << std::endl;
  }

  return 0;
}