// #include "random.h"

#include <chrono>
#include <iostream>
#include <memory>

uint64_t GetClockTimeNs() {
  auto ts = std::chrono::system_clock::now();
  int64_t system_t = std::chrono::duration_cast<std::chrono::nanoseconds>(ts.time_since_epoch()).count();
  return static_cast<uint64_t>(system_t);
}

// void Test() {
//   size_t size = 1024 * 1024 * 1024;
//   void *buf = malloc(size * sizeof(float));
//   auto start = GetClockTimeNs();
//   RandomNormal(0, reinterpret_cast<float *>(buf), size, 1.0, 1024.0);
//   auto cost = GetClockTimeNs() - start;
//   std::cout << buf << " cost : " << cost << " ns" << std::endl;
//   free(buf);
// }
struct Node {
  bool b_3;
  uint32_t u_2;
  bool b_1;
};

int main() {
  int use_small = 0;
  bool from_p = false;
  use_small = true || false ? -1 : -2;
  std::cout << "use small : " << use_small << std::endl;
  std::cout << sizeof(Node) << std::endl;
  bool b_1 = 1;
  uint32_t u_2 = 2;
  std::cout << sizeof(std::tie(b_1, u_2, b_1)) << std::endl;
  return 0;
}