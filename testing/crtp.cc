#include <iostream>
#include <memory>
#include <variant>
#include <cstdlib>
using namespace std;

template <typename Derived>
class Allocator {
 public:
  void *Malloc(size_t size) { return static_cast<Derived *>(this)->DoMalloc(size); }

  bool Free(void *addr) { return static_cast<const Derived *>(this)->DoFree(addr); }
};

class StandardAllocator : public Allocator<StandardAllocator> {
 public:
  void *DoMalloc(size_t size) { return malloc(size); }
  bool DoFree(void *addr) {
    free(addr);
    return true;
  }
};

class DefaultAllocator {
 public:
  void *allocate(size_t size) { return malloc(size); }
  bool deallocate(void *addr) {
    free(addr);
    return true;
  }
};

class EnhancedAllocator {
 public:
  void *allocate(size_t size) {
    cout << "malloc : " << size << endl;
    return malloc(size);
  }
  bool deallocate(void *addr) {
    cout << "free : " << addr << endl;
    free(addr);
    return true;
  }
};

using VariantAllocator = std::variant<DefaultAllocator, EnhancedAllocator>;

VariantAllocator TestVariant(bool flag) {
  if (flag) return DefaultAllocator();
  return EnhancedAllocator();
}

struct MultiVisitor {
  void operator()(int a, double b) const { std::cout << "Int + Double: " << a + b << std::endl; }

  void operator()(int a, bool b) const { std::cout << "Int + Bool: " << a << " " << std::boolalpha << b << std::endl; }

  void operator()(const std::string &a, double b) const {
    std::cout << "String + Double: " << a << " " << b << std::endl;
  }

  void operator()(const std::string &a, bool b) const {
    std::cout << "String + Bool: " << a << " " << std::boolalpha << b << std::endl;
  }
};

// 1. 定义不同分配器实现（无共同基类）
struct MallocAllocator {
  void *allocate(size_t size) {
    auto ret = std::malloc(size);
    cout << "Malloc allocator, size : " << size << ", ret : " << ret << endl;
    return ret;
  }
  void deallocate(void *ptr) {
    cout << "Malloc allocator, free : " << ptr << endl;
    std::free(ptr);
  }
};

struct PoolAllocator {
  void *allocate(size_t size) {
    cout << "PoolAllocator allocator" << endl;
    return nullptr;
  }
  void deallocate(void *ptr) { /* 池实现... */ }
};

struct ArenaAllocator {
  void *allocate(size_t size) { /* 区域分配器... */ return nullptr; }
  void deallocate(void *ptr) { /* 区域分配器... */ }
};

// 2. 定义分配器类型集合
using AllocatorVariant = std::variant<MallocAllocator, PoolAllocator, ArenaAllocator, DefaultAllocator, EnhancedAllocator>;

// 3. 定义操作分发器（替代虚函数）
struct AllocateOp {
  size_t size;
  // 正确写法
  template <typename T>
  void *operator()(T &alloc) {
    return alloc.allocate(size);
  }
};

struct DeallocateOp {
  void *ptr;
  template <typename T>
  void operator()(T &alloc) {
    alloc.deallocate(ptr);
  }
};

// 4. 统一接口类
class MemoryAllocator {
 public:
  // 编译时选择分配器类型
  template <typename AllocType>
  MemoryAllocator(AllocType alloc) : m_alloc(alloc) {}

  void *allocate(size_t size) { return std::visit(AllocateOp{size}, m_alloc); }

  void deallocate(void *ptr) { std::visit(DeallocateOp{ptr}, m_alloc); }

 private:
  AllocatorVariant m_alloc;
};

// 5. 配置工厂（根据配置创建实例）
MemoryAllocator create_allocator(const std::string &cfg) {
  if (cfg == "malloc") return MallocAllocator{};
  if (cfg == "pool") return PoolAllocator{};
  if (cfg == "arena") return ArenaAllocator{};
  if (cfg == "default") return DefaultAllocator{};
  if (cfg == "enhanced") return EnhancedAllocator{};
  throw std::runtime_error("Unknown allocator type");
}

// 使用示例
int main() {
  MemoryAllocator alloc = create_allocator("enhanced");

  void *mem = alloc.allocate(1024);
  alloc.deallocate(mem);
}

int main2() {
  // Allocator *allocator = new StandardAllocator();
  // void *ptr = allocator->Malloc(100);
  // cout << "ptr : " << ptr << endl;

  // auto allocator = TestVariant(true);
  // cout << "start visit" << endl;
  // std::visit(
  //           [](const auto &arg) {
  //             if constexpr (std::is_same_v<decltype(arg), const DefaultAllocator>) {
  //               std::cout << "DefaultAllocator: " << arg << std::endl;  // 输出：42
  //             } else if constexpr (std::is_same_v<decltype(arg), EnhancedAllocator>) {
  //               std::cout << "EnhancedAllocator: " << arg << std::endl;  // 输出：42
  //             }
  //             cout << "visiting " << endl;              // return arg.Malloc(size);
  //           },
  //           allocator);
  // cout << "allocator malloc : "
  //      << 1
  //      << endl;
  // auto allocator2 = TestVariant(false);
  // cout << "allocator malloc : "
  //      << std::visit(
  //           [](const auto &arg, size_t size) {
  //             if constexpr (std::is_same_v<decltype(arg), DefaultAllocator>) {
  //               std::cout << "DefaultAllocator: " << arg.value << std::endl;  // 输出：42
  //             } else if constexpr (std::is_same_v<decltype(arg), EnhancedAllocator>) {
  //               std::cout << "EnhancedAllocator: " << arg.value << std::endl;  // 输出：42
  //             }
  //             return arg.Malloc(size);
  //           },
  //           allocator2, 500)
  //      << endl;

  return 0;
}