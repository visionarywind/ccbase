#include <iostream>

template <typename T>
class RemovePointer {};

template <typename T>
class RemovePointer<T *> {
 public:
  typedef T Result;
};

template <typename T>
class TypeToID {
 public:
  typedef T SameAsT;
  static int const ID = 0x80000000;  // 用最高位表示它是一个指针
};

template <>
class TypeToID<int *> {
 public:
  static int const ID = 0x12345678;  // 给一个缺心眼的ID
};

void TemplateTest() {
  RemovePointer<float *>::Result x = 5.0f;
  std::cout << x << std::endl;
  std::cout << "ID of int*: " << TypeToID<int*>::ID << std::endl;
}