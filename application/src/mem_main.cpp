#include <iostream>
#include <chrono>

#include "Allocator.h"
#define OP

#ifndef OP
#include "mem_dynamic_allocator.h"
#else
#include "mem_optimized_allocator.h"
#endif

using namespace std;

inline int64_t Get() {
  auto now_time = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(now_time.time_since_epoch()).count();
}

int AllocTest() {
  DefaultAllocator allocator;
  int64_t cost = 0;
  void *addr;

  addr = allocator.Alloc(512);
  // allocator.Free(addr);
  for (int i = 0; i < 10000; i++) {
    // addr = allocator.Alloc(5120);
    auto start_time = Get();
    addr = allocator.Alloc(512);
    cost += Get() - start_time;
    // allocator.Free(addr);
  }
  cout << "allo - cost : " << cost / 10000.0 / 1000 << "us, addr : " << addr << endl;
  return 1;
}

int PoolTest() {
#ifdef OP
  DynamicMemPoolBestFitOpt pool;
#else
  DynamicMemPoolBestFit pool;
#endif
  DeviceMemPtr addr;
  int64_t cost = 0;
  pool.AllocTensorMem(512);
  // pool.FreeTensorMem(addr);
  for (int i = 0; i < 10000; i++) {
    // addr = pool.AllocTensorMem(5120);
    auto start_time = Get();
    addr = pool.AllocTensorMem(512);
    cost += Get() - start_time;
    // pool.FreeTensorMem(addr);
  }
  cout << "pool - cost : " << cost / 10000.0 / 1000 << "us, addr : " << addr << endl;

  return 1;
}

int main() {
  AllocTest();
  PoolTest();
  return 1;
}