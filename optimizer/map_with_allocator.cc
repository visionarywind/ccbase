#include "allocator.h"

#include <map>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

int main() {
  typedef Allocator<std::map<int, int>::value_type, 1024> MapMemoryPoolAllocator;

  std::thread t1 = std::thread([]() {
    auto start = GetTick();
    for (size_t i = 0; i < 1000; i++) {
      std::map<int, int, std::less<int>, MapMemoryPoolAllocator> m;
      for (int i = 0; i < 1000; i++) {
        m[i] = i;
      }
      //   for (int i = 0; i < 1000; i++) {
      //     m.erase(i);
      //   }
    }
    auto cost = GetTick() - start;
    printf("t1 cost : %lu ns.\n", cost);
  });

  std::thread t2 = std::thread([]() {
    auto start = GetTick();
    for (size_t i = 0; i < 1000; i++) {
      std::map<int, int, std::less<int>> m;
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