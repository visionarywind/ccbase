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

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <cstring>
#include <vector>
using namespace std;

// str: 要分割的字符串
// result: 保存分割结果的字符串数组
// delim: 分隔字符串
vector<string> split(const std::string &str, const std::string delim = ",") {
  vector<string> ret;
  char *buffer = new char[str.size() + 1];
  std::strcpy(buffer, str.c_str());
  char *tmp;
  char *p = strtok(buffer, delim.c_str(), &tmp);  // 第一次分割
  do {
    ret.push_back(p);  // 如果 p 为 nullptr，则将整个字符串作为结果
  } while ((p = strtok(nullptr, delim.c_str(), &tmp)) != nullptr);
  // strtok_r 为 strtok 的线程安全版本。
  delete[] buffer;
  return ret;
}

struct StatNode {
  StatNode(uint64_t start_time, uint64_t end_time, uint64_t device_addr, size_t size)
      : start_time_(start_time), end_time_(end_time), device_addr_(device_addr), size_(size) {}

  static StatNode *parse(const vector<string> &str) {
    stringstream ss;

    string start_time_str = str[0];
    ss << start_time_str;
    uint64_t start_time;
    ss >> start_time;
    // cout << start_time_str << ", start_time : " << start_time << endl;;

    string end_time_str = str[1];
    ss.clear();
    ss << end_time_str;
    uint64_t end_time;
    ss >> end_time;
    // cout << end_time_str << ", end_time : " << end_time << endl;

    string device_addr_str = str[2];
    stringstream hex_ss;
    hex_ss << std::hex << device_addr_str;
    uint64_t device_addr;
    hex_ss >> device_addr;
    // cout << device_addr_str << ", device_addr : " << device_addr << endl;

    string size_str = str[5];
    ss.clear();
    ss << size_str;
    size_t size;
    ss >> size;
    // cout << size_str << ", size : " << size << endl;

    return new StatNode(start_time, end_time, device_addr, size);
  }

  string to_string() {
    stringstream ss;
    ss << "start_time : " << start_time_ << ", end_time : " << end_time_ << ", device_addr : " << device_addr_
       << ", size : " << size_ << "";
    return ss.str();
  }

  uint64_t start_time_;
  uint64_t end_time_;
  uint64_t device_addr_;
  size_t size_;
};

struct Analyzer {
  bool process(StatNode *stat_node) {
    uint64_t cur_time = stat_node->start_time_;

    // Remove overtime nodes
    auto iter = alive_nodes.begin();
    while (iter != alive_nodes.end()) {
      auto node = iter->second;
      if (node->end_time_ <= cur_time) {
        iter = alive_nodes.erase(iter);
        delete node;
      } else {
        iter++;
      }
    }

    // Check range
    iter = alive_nodes.begin();
    auto start = stat_node->device_addr_;
    auto end = start + stat_node->size_;
    while (iter != alive_nodes.end()) {
      auto node = iter->second;
      auto node_start = node->device_addr_;
      auto node_end = node_start + node->size_;
      // not in range
      if (start >= node_end || end <= node_start) {
      } else {
        // in range
        cout << "Conflit - node : " << node->to_string() << ", stat_node : " << stat_node->to_string() << endl;
        return false;
      }

      iter++;
    }

    return true;
  }

  map<uint64_t, StatNode *> alive_nodes;
};

int ProcessCsv(int index) {
  string filename = "memory_block.csv";
  ifstream fin(filename.c_str());
  string strline;
  vector<string> lines;
  while (getline(fin, strline)) {
    lines.emplace_back(std::move(strline));
  }
  fin.close();
  // process

  Analyzer analyzer;

  size_t len = lines.size();
  for (size_t i = index; i < len; i++) {
    string cur = lines[i];
    vector<string> ret = split(cur);
    auto data = StatNode::parse(ret);
    if (!analyzer.process(data)) {
      cout << "Conflict!";
      break;
    }
  }
  // cout << "Done!\n";
  return 0;
}

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
    ProcessCsv(i + 1);
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
    ProcessCsv(i+1);
  }

  cout << ss.str().size() << endl;
  cout << "DefaultAllocator alloc cost : " << cost * 1.0 / count / 1000 << "us, addr : " << addr << endl;
  cout << "DefaultAllocator free cost : " << free_cost * 1.0 / free_count / 1000 << "us." << endl;
  return 1;
}

int MultimapTest(int count = 10000) {
  MultimapAllocator allocator;
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
  cout << "MultimapAllocator alloc cost : " << cost * 1.0 / count / 1000 << "us, addr : " << addr << endl;
  cout << "MultimapAllocator free cost : " << free_cost * 1.0 / free_count / 1000 << "us." << endl;
  return 1;
}

int PoolTest(int count = 1000) {
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
    ProcessCsv(i + 1);
    auto start_time = Get();
    addr = pool.AllocTensorMem(512 + i * 128);
    cost += Get() - start_time;
    ss << addr << ", ";
    if (set.count(addr) == 0) {
      set.emplace(addr);
      vec.emplace_back(addr);
    }

    auto tmp = pool.AllocTensorMem(512 + i * 128 * 10);
    // pool.FreeTensorMem(tmp);
    // pool.FreeTensorMem(addr);
  }

  cout << "start to free" << endl;
  int free_count = count;
  int64_t free_cost = 0;
  for (int i = 0; i < free_count; i++) {
    auto free_start = Get();
    pool.FreeTensorMem(vec[i]);
    free_cost += Get() - free_start;
    ProcessCsv(i+1);
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

int SkipAllocTest(int count = 10000) {
  SkipListAllocator kAllocator;
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
  cout << "SkipListAllocator alloc cost : " << cost * 1.0 / count / 1000 << "us, addr : " << addr << endl;
  cout << "SkipListAllocator free cost : " << free_cost * 1.0 / free_count / 1000 << "us." << endl;
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
  // warm up cache.
  ProcessCsv(1);
  
  PoolTest(count);
  AllocTest(count);
  // MultimapTest(count);
  // for (int i = 0; i < 100000; i++) SkipAllocTest(count);
  // SkipAllocTest(count);
  // SkipAllocTest(count);

  // SkipListAllocator kAllocator;
  // vector<void *> addresses;
  // for (int i = 0; i < 10000; i++) {
  //   auto addr1 = kAllocator.Alloc(1 + i * 512);
  //   auto addr2 = kAllocator.Alloc(2 + i * 512);
  //   auto addr3 = kAllocator.Alloc(3 + i * 512);
  //   auto addr4 = kAllocator.Alloc(4 + i * 512);
  //   auto addr5 = kAllocator.Alloc(5 + i * 512);
  //   kAllocator.Free(addr1);
  //   addresses.emplace_back(addr2);
  //   addresses.emplace_back(addr3);
  //   addresses.emplace_back(addr4);
  //   addresses.emplace_back(addr5);
  //   // kAllocator.Dump();
  //   // kAllocator.Free(addr2);
  //   // kAllocator.Free(addr3);
  //   // // kAllocator.Dump();
  //   // // cout << "====free next level====" << endl;
  //   // kAllocator.Free(addr4);
  //   // kAllocator.Free(addr5);
  //   // kAllocator.Dump();
  //   // kAllocator.Dump();
  //   // kAllocator.Dump();
  // }
  // for (auto addr : addresses) {
  //   kAllocator.Free(addr);
  // }

  return 1;
}