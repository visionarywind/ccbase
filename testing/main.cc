#include "random.h"

#include <chrono>
#include <iostream>
#include <memory>

uint64_t GetClockTimeNs() {
  auto ts = std::chrono::system_clock::now();
  int64_t system_t = std::chrono::duration_cast<std::chrono::nanoseconds>(ts.time_since_epoch()).count();
  return static_cast<uint64_t>(system_t);
}

int main() {
  size_t size = 1024 * 1024 * 1024;
  void *buf = malloc(size * sizeof(float));
  auto start = GetClockTimeNs();
  RandomNormal(0, reinterpret_cast<float *>(buf), size, 1.0, 1024.0);
  auto cost = GetClockTimeNs() - start;
  std::cout << buf << " cost : " << cost << " ns" << std::endl;
  free(buf);
  return 0;
}