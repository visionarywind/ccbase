#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <list>
#include <set>
#include <chrono>
#include <random>

// ===================== macOS 需要兼容pmr实现，当前版本有问题，未调试通过 =====================
#if defined(__APPLE__) || !defined(__has_include) || !__has_include(<memory_resource>)
class memory_resource {
 public:
  virtual ~memory_resource() = default;
  virtual void *allocate(size_t bytes, size_t alignment) = 0;
  virtual void deallocate(void *p, size_t bytes, size_t alignment) = 0;

 private:
  virtual void *do_allocate(std::size_t bytes, std::size_t alignment) {
    std::cout << "do_allocate bytes : " << bytes << std::endl;
    return nullptr;
  }
  virtual void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) {
    std::cout << "do_deallocate bytes : " << bytes << ", p : " << p << std::endl;
  }
  virtual bool do_is_equal(const memory_resource &other) const noexcept {
    std::cout << "do_allocate bytes"  << std::endl;
    return true;
  }
};

class new_delete_resource : public memory_resource {
 public:
  static memory_resource *instance() {
    static new_delete_resource inst;
    return &inst;
  }

  void *allocate(size_t bytes, size_t alignment) override {
    void *p = nullptr;
    if (posix_memalign(&p, alignment, bytes) != 0) {
      throw std::bad_alloc();
    }
    return p;
  }

  void deallocate(void *p, size_t, size_t) override { free(p); }
};

class monotonic_buffer_resource : public memory_resource {
  void *buffer_;
  size_t size_;
  size_t used_ = 0;
  memory_resource *upstream_;

 public:
  monotonic_buffer_resource(void *buffer, size_t size, memory_resource *upstream = new_delete_resource::instance())
      : buffer_(buffer), size_(size), upstream_(upstream) {}

  ~monotonic_buffer_resource() {}

  void *allocate(size_t bytes, size_t alignment) override {
    std::cout << "allocate bytes : " << bytes << ", alignment : " << alignment << std::endl;
    if (size_ - used_ >= bytes) {
      void *ptr = static_cast<char *>(buffer_) + used_;
      size_t space = size_ - used_;

      if (std::align(alignment, bytes, ptr, space)) {
        used_ = size_ - space;
        return ptr;
      }
    }

    std::cout << "out of space" << std::endl;
    return upstream_->allocate(bytes, alignment);
  }

  void deallocate(void *, size_t, size_t) override {
    // 单调缓冲区不释放单个对象
  }

  void reset() { used_ = 0; }

  size_t remaining() const { return size_ - used_; }
};

template <typename T>
class polymorphic_allocator {
  memory_resource *resource_;

 public:
  using value_type = T;

  polymorphic_allocator(memory_resource *r) noexcept : resource_(r) {}

  template <typename U>
  polymorphic_allocator(const polymorphic_allocator<U> &other) noexcept : resource_(other.resource()) {}

  T *allocate(size_t n) { return static_cast<T *>(resource_->allocate(n * sizeof(T), alignof(T))); }

  void deallocate(T *p, size_t n) { resource_->deallocate(p, n * sizeof(T), alignof(T)); }

  memory_resource *resource() const { return resource_; }

  bool operator==(const polymorphic_allocator &other) const { return resource_ == other.resource_; }

  bool operator!=(const polymorphic_allocator &other) const { return !(*this == other); }
};

template <typename T>
using vector = std::vector<T, polymorphic_allocator<T>>;

template <typename T>
using list = std::list<T, polymorphic_allocator<T>>;

template <typename K, typename V, typename Compare = std::less<K>>
using map = std::map<K, V, Compare, polymorphic_allocator<std::pair<const K, V>>>;

template <typename T, typename Compare = std::less<T>>
using set = std::set<T, Compare, polymorphic_allocator<T>>;

using pmr_string = std::basic_string<char, std::char_traits<char>, polymorphic_allocator<char>>;

namespace pmr {
using ::list;
using ::map;
using ::memory_resource;
using ::monotonic_buffer_resource;
using ::new_delete_resource;
using ::pmr_string;
using ::polymorphic_allocator;
using ::set;
using ::vector;

inline memory_resource *get_default_resource() { return new_delete_resource::instance(); }
}  // namespace pmr

#else
#include <memory_resource>
namespace pmr = std::pmr;
#endif

class InstrumentedResource : public pmr::memory_resource {
  pmr::memory_resource *upstream_;
  size_t alloc_count_ = 0;
  size_t dealloc_count_ = 0;
  size_t total_allocated_ = 0;

 public:
  explicit InstrumentedResource(pmr::memory_resource *upstream = pmr::get_default_resource()) : upstream_(upstream) {}

  size_t allocation_count() const { return alloc_count_; }
  size_t deallocation_count() const { return dealloc_count_; }
  size_t total_allocated() const { return total_allocated_; }

 protected:
  void *do_allocate(size_t bytes, size_t alignment) override {
    void *p = upstream_->allocate(bytes, alignment);
    alloc_count_++;
    total_allocated_ += bytes;
    return p;
  }

  void do_deallocate(void *p, size_t bytes, size_t alignment) override {
    upstream_->deallocate(p, bytes, alignment);
    dealloc_count_++;
  }

  bool do_is_equal(const pmr::memory_resource &other) const noexcept override { return this == &other; }
};

template <typename Container>
void test_container_performance(const char *name, Container &container, const std::vector<int> &data) {
  auto start = std::chrono::high_resolution_clock::now();

  for (int value : data) {
    std::cout << "insert value : " << value << std::endl;
    container.insert(value);
  }

  size_t found = 0;
  for (int value : data) {
    if (container.find(value) != container.end()) {
      found++;
    }
  }

  for (int value : data) {
    container.erase(value);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

  std::cout << name << " performance: " << duration << " ms\n";
  std::cout << "  - Elements: " << data.size() << ", Found: " << found << "\n";
}

int main() {
  const size_t NUM_ELEMENTS = 50000;
  std::vector<int> data;

  std::random_device rd;
  std::mt19937 gen(rd());
  for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
    data.push_back(static_cast<int>(gen()));
  }

  std::cout << "===== Polymorphic Allocator Demo (macOS compatible) =====\n";
  std::cout << "Testing with " << NUM_ELEMENTS << " elements\n\n";

  {
    std::set<int> default_set;
    test_container_performance("Default allocator", default_set, data);
  }

  {
    const size_t BUFFER_SIZE = 2 * 1024 * 1024;  // 2MB
    std::vector<char> buffer(BUFFER_SIZE);

    pmr::monotonic_buffer_resource pool{buffer.data(), buffer.size(), pmr::get_default_resource()};

    pmr::set<int> custom_set{&pool};
    test_container_performance("Monotonic buffer", custom_set, data);

    std::cout << "  - Buffer used: " << (BUFFER_SIZE - pool.remaining()) << " bytes\n";
  }

  #ifdef T3
  // 3. 使用带统计的自定义资源
  {
    InstrumentedResource custom_res;
    pmr::set<int> custom_set{&custom_res};

    test_container_performance("Instrumented resource", custom_set, data);

    std::cout << "  - Allocations: " << custom_res.allocation_count()
              << ", Deallocations: " << custom_res.deallocation_count()
              << ", Total allocated: " << custom_res.total_allocated() << " bytes\n";
  }
  #endif

  // 4. 使用多态分配器与字符串
  {
    // 创建内存池
    const size_t BUFFER_SIZE = 1 * 1024 * 1024;  // 1MB
    std::vector<char> buffer(BUFFER_SIZE);
    pmr::monotonic_buffer_resource pool{buffer.data(), buffer.size()};

    // 使用相同内存资源的字符串和集合
    pmr::polymorphic_allocator<char> alloc{&pool};

    pmr::set<pmr::pmr_string> string_set{alloc};

    auto start = std::chrono::high_resolution_clock::now();

    // 插入字符串
    for (int i = 0; i < 10000; ++i) {
      string_set.insert(pmr::pmr_string("String_" + std::to_string(i), alloc));
    }

    // 查找操作
    size_t found = 0;
    for (int i = 0; i < 10000; ++i) {
      pmr::pmr_string key("String_" + std::to_string(i), alloc);
      if (string_set.find(key) != string_set.end()) {
        found++;
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "String set performance: " << duration << " ms\n";
    std::cout << "  - Elements: 10000, Found: " << found << "\n";
    std::cout << "  - Pool used: " << (BUFFER_SIZE - pool.remaining()) << " bytes\n";
  }

  return 0;
}