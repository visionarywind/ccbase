#include <iostream>
#include <memory>
#include <iostream>
#include <execinfo.h>

#include "ms.hpp"

struct MyObject : Allocator {
  void *operator new(size_t size) {
    // allocatorMap[allocator_name_] += size;
    std::cout << "MyObject : new " << size << " bytes. Total allocated so far: " << totalAllocatedMemory << " bytes."
              << std::endl;
    return malloc(size);
  }

  void operator delete(void *ptr, size_t size) {
    std::cout << "MyObject : delete " << size << " bytes. Total freed so far: " << totalFreedMemory << " bytes."
              << std::endl;
    free(ptr);
  }

  int value;
  MyObject(int val) : value(val) { vec.emplace_back(val); }
  std::vector<int> vec;
};

struct Node : MyObject {
  Node(int val) : MyObject(val), node_value_(val) { node_vec_.emplace_back(val); }
  int node_value_;
  std::vector<int> node_vec_;
};

struct Derived : Node {
  Derived(int val) : Node(val), derived_value_(val) { derived_vec_.emplace_back(val); }
  int derived_value_;
  std::vector<int> derived_vec_;
};

int main() {
  // 使用自定义分配器
  // auto my_shared_ptr = std::allocate_shared<MyObject>(CustomAllocator<MyObject>(), 42);
  //   auto my_shared_ptr = ms_make_shared<MyObject>(42);
  //   my_shared_ptr->value = 43;
  //   std::cout << "Value: " << my_shared_ptr->value << std::endl;

  auto ptr = ms_make_shared(MyObject, 2);
  ptr->value = 3;
  std::cout << "MyObject : " << ptr->value << std::endl;

  /*
    auto node = ms_make_shared(Node, 2);
    node->value = 3;
    std::cout << "Node : " << node->value << std::endl;

     auto derived = ms_make_shared(Derived, 2);
     derived->value = 3;
     std::cout << "Derived : " << derived->value << std::endl;
  */

  DumpAllocator();

  return 0;
}