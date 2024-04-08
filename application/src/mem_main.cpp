#include <iostream>
#include <chrono>
#include <set>
#include <sstream>

#include "Allocator.h"
// #define OP

#ifndef OP
#include "mem_dynamic_allocator.h"
#else
#include "mem_optimized_allocator.h"
#endif

// #include "sorted_list.h"

using namespace std;

inline int64_t Get() {
  auto now_time = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(now_time.time_since_epoch()).count();
}

int AllocTest(int count = 10000) {
  DefaultAllocator allocator;
  int64_t cost = 0;
  void *addr;
  stringstream ss;

  addr = allocator.Alloc(512);
  set<DeviceMemPtr> set;
  vector<DeviceMemPtr> vec;
  for (int i = 0; i < count; i++) {
    // addr = allocator.Alloc(5120);
    auto start_time = Get();
    addr = allocator.Alloc(512 + i * 128);
    cost += Get() - start_time;
    ss << addr << ", ";
    // allocator.Free(addr);
    if (set.count(addr) == 0) {
      set.emplace(addr);
      vec.emplace_back(addr);
    }

    auto tmp = allocator.Alloc(512 + i * 128 * 10);
    allocator.Free(tmp);
  }

  cout << "new start to free" << endl;
  int free_count = count;
  int64_t free_cost = 0;
  for (int i = 0; i < free_count; i++) {
    auto free_start = Get();
    allocator.Free(vec[i]);
    free_cost += Get() - free_start;
  }

  cout << ss.str().size() << endl;
  cout << "new alloc cost : " << cost * 1.0 / count / 1000 << "us, addr : " << addr << endl;
  cout << "new free cost : " << free_cost * 1.0 / free_count / 1000 << "us." << endl;
  return 1;
}

int PoolTest(int count = 10000) {
#ifdef OP
  DynamicMemPoolBestFitOpt pool;
#else
  DynamicMemPoolBestFit pool;
#endif
  DeviceMemPtr addr;
  int64_t cost = 0;

  stringstream ss;
  pool.AllocTensorMem(512);
  set<DeviceMemPtr> set;
  vector<DeviceMemPtr> vec;
  for (int i = 0; i < count; i++) {
    // addr = pool.AllocTensorMem(5120);
    auto start_time = Get();
    addr = pool.AllocTensorMem(512 + i * 128);
    cost += Get() - start_time;
    ss << addr << ", ";
    if (set.count(addr) == 0) {
      set.emplace(addr);
      vec.emplace_back(addr);
    }

    auto tmp = pool.AllocTensorMem(512 + i * 128 * 10);
    pool.FreeTensorMem(tmp);
    // pool.FreeTensorMem(addr);
  }

  cout << "start to free" << endl;
  int free_count = count;
  int64_t free_cost = 0;
  for (int i = 0; i < free_count; i++) {
    auto free_start = Get();
    pool.FreeTensorMem(vec[i]);
    free_cost += Get() - free_start;
  }
  cout << ss.str().size() << endl;
  pool.DumpDynamicMemPoolStateInfo();
  pool.DumpDynamicMemPoolDebugInfo();
  cout << "old alloc cost : " << cost * 1.0 / count / 1000 << "us, addr : " << addr << endl;
  cout << "old free cost : " << free_cost * 1.0 / free_count / 1000 << "us." << endl;
  return 1;
}

int main2() {
  PoolTest();
  cout << "===========================================" << endl;
  AllocTest();
  cout << "===========================================" << endl;
  PoolTest();
  cout << "===========================================" << endl;
  AllocTest();
  return 1;
}

SkipListAllocator kAllocator;

int SkipAllocTest(int count = 10000) {
  int64_t cost = 0;
  void *addr;
  stringstream ss;

  addr = kAllocator.Alloc(512);
  set<DeviceMemPtr> set;
  vector<DeviceMemPtr> vec;
  for (int i = 0; i < count; i++) {
    // addr = allocator.Alloc(5120);
    auto start_time = Get();
    addr = kAllocator.Alloc(512 + i * 128);
    cost += Get() - start_time;
    ss << addr << ", ";
    // allocator.Free(addr);
    if (set.count(addr) == 0) {
      set.emplace(addr);
      vec.emplace_back(addr);
    }

    auto tmp = kAllocator.Alloc(512 + i * 128 * 10);
    kAllocator.Free(tmp);
  }

  cout << "skiplist start to free" << endl;
  int free_count = count;
  int64_t free_cost = 0;
  for (int i = 0; i < free_count; i++) {
    auto free_start = Get();
    kAllocator.Free(vec[i]);
    free_cost += Get() - free_start;
  }

  cout << ss.str().size() << endl;
  cout << "skiplist alloc cost : " << cost * 1.0 / count / 1000 << "us, addr : " << addr << endl;
  cout << "skiplist free cost : " << free_cost * 1.0 / free_count / 1000 << "us." << endl;
  return 1;
}

// void TestSortedList() {
//   SortedList<size_t, size_t> skip_list;
//   vector<size_t> blocks;
//   for (int i = 0; i < 10; i++) {
//     blocks.emplace_back(i);
//   }
//   for (auto block : blocks) {
//     skip_list.Insert(block, block);
//   }
//   cout << "list size : " << skip_list.Size() << endl;

//   for (auto block : blocks) {
//     skip_list.Remove(block, block);
//   }
//   cout << "list size : " << skip_list.Size() << endl;
// }

int main() {
  int count = 100000;
  // PoolTest(count);
  // AllocTest(count);
  // for (int i = 0; i < 100000; i++) SkipAllocTest(count);
  SkipAllocTest(count);
  // SkipAllocTest(count);
  return 1;
}