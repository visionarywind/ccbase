#include <vector>
#include <stdio.h>
#include <stdlib.h>

template <typename T>
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

int main() {
  std::vector<int, MyAllocator<int>> v;
  for (int i = 0; i < 1000; i++) {
    v.push_back(i);
  }
  return 0;
}