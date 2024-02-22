#include <iostream>
#include <chrono>

#include "mem_dynamic_allocator.h"

using namespace std;

inline int64_t Get() {
  auto now_time = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(now_time.time_since_epoch()).count();
}

int main() {
  DynamicMemPoolBestFit pool;
  DeviceMemPtr addr;
  int64_t cost = 0;
  for (int i = 0; i < 10000; i++) {
    auto start_time = Get();
    addr = pool.AllocTensorMem(512);
    cost += Get() - start_time;
    // pool.AllocTensorMem(512);
    // pool.AllocTensorMem(512);
    // pool.AllocTensorMem(5120);
    // pool.AllocTensorMem(5120);
    // pool.AllocTensorMem(5120);
    pool.FreeTensorMem(addr);
  }
  cout << "cost : " << cost / 10000.0 / 1000 << "us, addr : " << addr << endl;

  return 1;
}