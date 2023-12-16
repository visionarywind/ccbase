#include <iostream>
#include <map>
#include <vector>
using namespace std;
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

// T can not be deduced
template <typename T, typename C>
T &Get(C &container, int id) {
  auto iter = container.find(id);
  if (iter != container.end()) {
    return iter->second;
  }
  container.emplace(id, T());
  return container[id];
}

void TemplateTest() {
  RemovePointer<float *>::Result x = 5.0f;
  std::cout << x << std::endl;
  std::cout << "ID of int*: " << TypeToID<int*>::ID << std::endl;

  map<int, vector<int>> m{{1, {1}}, {2, {2}}};
  int key = 1;
  vector<int> vec = Get<vector<int>>(m, key);
  cout << "vec size : " << vec.size() << endl;
}