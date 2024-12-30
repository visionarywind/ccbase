#include "allocator.h"

#include <array>
#include <unordered_map>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <iostream>
#include <unordered_map>
#include <memory>

struct IntHash {
  template <class T1, class T2>
  std::size_t operator()(std::pair<T1, T2> const &v) const {
    return std::hash<T1>()(v);
  }
};

// todo, not passthrough current allocator
int main() {
  std::thread t1 = std::thread([&]() {
    auto start = GetTick();
    for (size_t i = 0; i < 1000; i++) {
      using MapAllocator = PoolAllocator<std::pair<const int, int>>;
      IntHash hash;
      std::unordered_map<int, int, IntHash, std::equal_to<const int>, MapAllocator> m(
        0, hash, std::equal_to<const int>(), MapAllocator(1024));
      //   CustomAllocator<std::pair<const int, int>> allocator;
      //   std::unordered_map<int, int, std::hash<int>, std::equal_to<int>, CustomAllocator<std::pair<const int, int>>>
      //   m(
      //     0, std::hash<int>(), std::equal_to<int>(), allocator);
      for (int i = 0; i < 1000; i++) {
        m[i] = i;
      }
      for (int i = 0; i < 1000; i++) {
        m.erase(i);
      }
    }
    auto cost = GetTick() - start;
    printf("t1 cost : %lu ns.\n", cost);
  });

  std::thread t2 = std::thread([]() {
    auto start = GetTick();
    for (size_t i = 0; i < 1000; i++) {
      std::unordered_map<int, int> m;
      for (int i = 0; i < 1000; i++) {
        m[i] = i;
      }
      //   for (int i = 0; i < 1000; i++) {
      //     m.erase(i);
      //   }
    }
    auto cost = GetTick() - start;
    printf("t2 cost : %lu ns.\n", cost);
  });

  t1.join();
  t2.join();

  return 0;
}