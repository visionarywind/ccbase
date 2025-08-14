#include <iostream>
#include <map>
using namespace std;

template <typename K, typename V>
struct ConstMap {
  ConstMap() = default;

  V &operator[](const K &key) const { return proxy_[key]; }

  mutable std::map<K, V> proxy_;
};

struct ConstA {
  void View(int index) const { const_map_[index]++; }

  ConstMap<int, int> const_map_;
};

int main() {
  ConstA const_a;
  int index = 0;
  cout << const_a.const_map_[index] << endl;
  const_a.View(index);
  cout << const_a.const_map_[index] << endl;
  return 0;
}