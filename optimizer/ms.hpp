
#include <iostream>
#include <memory>
#include <map>
#include <string>


size_t totalAllocatedMemory = 0;
size_t totalFreedMemory = 0;

/*
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

void printStackTrace() {
  void *buffer[64];
  int nptrs = backtrace(buffer, 64);
  char **strings = backtrace_symbols(buffer, nptrs);
  std::cout << "Stack trace:" << std::endl;
  for (int i = 0; i < nptrs; ++i) {
    std::cout << strings[i] << std::endl;
  }
  free(strings);
}

void printUpperFunctionName() {
  void *buffer[100];
  int nptrs = backtrace(buffer, 100);
  char **strings = backtrace_symbols(buffer, nptrs);
  if (nptrs > 1) {
    // 假设函数名在':'之后，提取函数名
    char *funcName = strtok(strings[1], "a.out");
    funcName = strtok(NULL, " ");
    std::cout << "Upper function name: " << funcName << std::endl;
  }
  free(strings);
}

static std::map<std::string, size_t> allocatorMap;

struct Allocator {
    Allocator() {
        allocator_name_ = __FILE__ + std::to_string(__LINE__);
    }
    
    void *operator new(size_t size) {
        // allocatorMap[allocator_name_] += size;
        std::cout << "Allocator : new " << size << " bytes. Total allocated so far: " << totalAllocatedMemory << " bytes."
                    << std::endl;
        return malloc(size);
    }

    void operator delete(void *ptr, size_t size) {
        std::cout << "Allocator : delete " << size << " bytes. Total freed so far: " << totalFreedMemory << " bytes." << std::endl;
        free(ptr);
    }

    std::string allocator_name_;
};

void DumpAllocator() {
    for (auto &it : allocatorMap) {
        std::cout << "allocator: " << it.first << ", size: " << it.second << std::endl;
    }
}

template <typename T>
class CustomAllocator {
 public:
  using value_type = T;

  CustomAllocator(const std::string file_name) : file_name_(file_name) {}

  template <typename U>
  CustomAllocator(const CustomAllocator<U> &other) : file_name_(other.file_name_) {}

  T *allocate(std::size_t n) {
    std::cout << "CustomAllocator : allocating " << n << " element(s) of size " << sizeof(T) << std::endl;
    // printUpperFunctionName();
    allocatorMap[file_name_] += n * sizeof(T);
    return static_cast<T *>(::operator new(n * sizeof(T)));
  }

  void deallocate(T *p, std::size_t) {
    std::cout << "CustomAllocator : deallocating memory" << std::endl;
    // printStackTrace();
    ::operator delete(p);
  }

  std::string file_name_;
};

template <typename T, typename U>
bool operator==(const CustomAllocator<T> &, const CustomAllocator<U> &) {
  return true;
}

template <typename T, typename U>
bool operator!=(const CustomAllocator<T> &, const CustomAllocator<U> &) {
  return false;
}

template <typename T, typename... Args>
std::shared_ptr<T> ms_make_shared(Args &&...args) {
  // std::cout << "line : " << __LINE__ << std::endl;
  return std::allocate_shared<T>(CustomAllocator<T>(__FILE__), std::forward<Args>(args)...);
}

template<typename T, typename... Args>
std::shared_ptr<T> custom_make_shared(const std::string &file, size_t line, Args&&... args) {
    std::cout << "Creating instance of " << typeid(T).name() << ", file : " << file << ", line : " << line << std::endl;
    // std::cout << "" << typeid(T) << std::endl;
    return std::allocate_shared<T>(CustomAllocator<T>(typeid(T).name()), std::forward<Args>(args)...);
    // return std::make_shared<T>(std::forward<Args>(args)...);
}

#define DEBUG
#ifdef DEBUG
#define ms_make_shared(T,...) \
    custom_make_shared<T>(__FILE__, __LINE__, __VA_ARGS__)
    // do { \
    //     std::cout << "Created shared_ptr for type " << typeid(T).name() << " in file " << __FILE__ << " at line " << __LINE__ << std::endl; \
    //     return std::allocate_shared<T>(CustomAllocator<T>(), std::forward<Args>(args)...); \
    // } while (0);
#else
#define ms_make_shared(T,...) std::make_shared<T>(__VA_ARGS__)
#endif



template<typename T>
class MyAllocator : public std::allocator<T> {
public:
  template <typename U>
  struct rebind {
    typedef MyAllocator<U> other;
  };

 public:
  T *allocate(size_t n, const void *hint = 0) {
    printf("allocate(%lu, %p)\n", n, hint);
    return (T *)malloc(n * sizeof(T));
  }

  void deallocate(T *ptr, size_t n) {
    printf("deallocate(%p, %lu)\n", ptr, n);
    free(ptr);
  }
};

// 模板特化std::allocator_traits
template<typename T>
struct std::allocator_traits<MyAllocator<T>> {
    using allocator_type = MyAllocator<T>;
    using value_type = typename MyAllocator<T>::value_type;
    using pointer = typename MyAllocator<T>::pointer;
    using const_pointer = typename MyAllocator<T>::const_pointer;
    using void_pointer = void*;
    using const_void_pointer = const void*;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;
    template<typename U>
    struct rebind {
        using other = MyAllocator<U>;
    };
    static pointer allocate(allocator_type& a, size_type n) {
        return a.allocate(n);
    }
    static void deallocate(allocator_type& a, pointer p, size_type n) {
        a.deallocate(p, n);
    }
    // 其他方法的实现根据std::allocator_traits的要求
};