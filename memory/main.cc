#include "memory_pool.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <type_traits>

using namespace std;

std::vector<std::string> split(std::string &s, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

template <typename T>
T parse(std::string &s) {
  std::stringstream sstream(s);
  T ans;
  sstream >> ans;
  return ans;
}

struct MemoryBlock {
  MemoryBlock(std::string &block_string) {
    auto &&elements = split(block_string, ",");
    start_time_stamp = parse<size_t>(elements[0]);
    end_time_stamp = parse<size_t>(elements[1]);
    device_addr = parse<void *>(elements[2]);
    stream_id = parse<uint32_t>(elements[3]);
    pool_type = parse<std::string>(elements[4]);
    size = parse<size_t>(elements[5]);
    actual_peak_mem = parse<size_t>(elements[6]);
    type = parse<std::string>(elements[9]);
  }

  size_t start_time_stamp;
  size_t end_time_stamp;
  void *device_addr;
  uint32_t stream_id;
  std::string pool_type;
  size_t size;
  size_t actual_peak_mem;
  // std::string file_name;
  // size_t line_num;
  std::string type;

  bool IsPersistent() { return type == "ConstantValue" || type == "Weight"; }

  std::string ToJson() {
    JsonBuilder builder;
    builder.Append("start_time_stamp", start_time_stamp);
    builder.Append("end_time_stamp", end_time_stamp);
    builder.Append("device_addr", device_addr);
    builder.Append("stream_id", stream_id);
    builder.Append("pool_type", pool_type);
    builder.Append("size", size);
    builder.Append("actual_peak_mem", actual_peak_mem);
    builder.Append("type", type);
    return builder.ToString();
  }
};

void Trace() {
  MemoryPool *pool = new MemoryPool();

  ifstream in("run.log");

  if (in) {
    string line;
    size_t index = 0;
    std::map<size_t, void *> to_free_map;

    // Traverse memory block csv.
    while (getline(in, line)) {
      index++;
      if (index == 1) {
        cout << "skip first line : " << line << endl;
        continue;
      }
      MemoryBlock block(line);
      // cout << "process line : " << index - 1 << ", content : " << line << ", block : " << block.ToJson() << endl;
      void *addr = pool->AllocTensorMem(block.size, block.IsPersistent());
      to_free_map.emplace(block.end_time_stamp, addr);
      // pool->DumpDynamicMemPoolStateInfo();

      for (auto it = to_free_map.begin(); it != to_free_map.end();) {
        if (it->first < block.start_time_stamp) {
          pool->FreeTensorMem(it->second);
          it = to_free_map.erase(it);
        } else {
          it++;
        }
      }
    }
  } else {
    cout << "no such file" << endl;
  }
  in.close();

  pool->DumpDynamicMemPoolStateInfo();
  delete pool;
}

struct Buf {
  void *addr;
};

std::vector<Buf *> bufs;

void *alloc() {
  Buf *buf = new Buf();
  buf->addr = malloc(1024);
  bufs.push_back(buf);
  std::cout << "buf addr : " << buf->addr << ", p " << &(buf->addr) << std::endl;
  return buf->addr;
}


std::vector<size_t> BuildPattern(const std::string_view &pattern) {
  size_t n = pattern.length();
  std::vector<size_t> ans(n, n - 1);
  size_t j = n - 1;
  for (size_t i = n - 2; i < SIZE_MAX; i--) {
    while (j < n - 1 && pattern[i] != pattern[j]) {
      j = ans[j + 1];
    }
    if (pattern[i] == pattern[j]) {
      j--;
    }
    ans[i] = j;
  }
  return ans;
}

std::pair<size_t, size_t> TailSubStr(const std::string_view &src_str, const std::string_view &sub_str) {
  if (sub_str.empty()) {
    return {0, 0};
  }
  if (src_str.empty() || src_str.length() < sub_str.length()) {
    return {SIZE_MAX, SIZE_MAX};
  }

  std::vector<size_t> pattern = BuildPattern(sub_str);
  size_t j = pattern.size() - 1;
  for (size_t i = src_str.length() - 1; i < SIZE_MAX; i--) {
    while (j < pattern.size() - 1 && src_str[i] != sub_str[j]) {
      j = pattern[j + 1];
    }
    if (src_str[i] == sub_str[j]) {
      j--;
    }
    if (j == SIZE_MAX) {
      return {i, i + sub_str.length() - 1};
    }
  }
  return {SIZE_MAX, SIZE_MAX};
}

bool MatchOp(const std::string_view &src, const std::string_view &opname) {
  if (opname.empty()) {
    return false;
  }

  auto [start, end] = TailSubStr(src, opname);
  if (start == SIZE_MAX) {
    return false;
  }
  const size_t prefix_index = 1;
  if (start > 0 && src[start - prefix_index] != '/') {
    return false;
  }

  if (end < src.length() - 1) {
    const size_t suffix_len_limit = 4;
    const size_t delimiter_index = 1;
    const size_t o_index = 2;
    const size_t p_index = 3;
    if (end + suffix_len_limit >= src.length() || src[end + delimiter_index] != '-' || src[end + o_index] != 'o' ||
        src[end + p_index] != 'p') {
      return false;
    }
    for (size_t i = end + suffix_len_limit; i < src.length(); i++) {
      if (!std::isdigit(src[i])) {
        return false;
      }
    }
  }
  return true;
}

void test() {
  auto addr = alloc();
  std::cout << "addr : " << addr << ", p " << &addr << std::endl;
  std::cout << bufs[0]->addr << ", p " << &(bufs[0]->addr) << std::endl;
  bufs[0]->addr = nullptr;
  std::cout << "addr : " << addr << ", p " << &addr << std::endl;
  std::cout << bufs[0]->addr << ", p " << &(bufs[0]->addr) << std::endl;


  std::cout << MatchOp("Pipeline/WithDynamic/A-op0123", "") << std::endl;
  std::cout << MatchOp("Pipeline/WithDynamic/Add-op0123", "Reshape") << std::endl;
  std::cout << MatchOp("Reshasdadasdpe-op1", "Reshape") << std::endl;
  std::cout << MatchOp("Reshape-op1", "Reshape") << std::endl;
  std::cout << MatchOp("Reshape-opx", "Reshape") << std::endl;
  std::cout << MatchOp("Reshape", "Reshape") << std::endl;
  std::cout << MatchOp("Pipeline/WithDynamic/Reshape-op123", "Reshape") << std::endl; 
  std::cout << MatchOp("Pipeline/WithDynamic/sdReshape-op0123", "Reshape") << std::endl; 
  std::cout << MatchOp("Pipeline/WithDynamicReshape/Add-op0123", "Reshape") << std::endl;
}

struct Addr {
  Addr() {}
  void *addr;
};

struct IpAddr : Addr {
  IpAddr() = default;
  int version;
};

struct PlainObj {
  // std::shared_ptr<std::map<int, int>> mapping;
  std::shared_ptr<void> addr;
};

int main() {
  MemoryPool *pool = new MemoryPool();
  std::vector<void *> addrs;
  for (size_t i = 0; i < 10; i++) {
    void *addr = pool->AllocTensorMem(1024 * 1024 * 1024, false);
    addrs.push_back(addr);
  }
  for (auto addr : addrs) {
    // pool->FreeTensorMem(addr);
  }
  // std::cout << "Before release " << addrs[0] << std::endl;
  // pool->FreeTensorMem(addrs[0]);
  // std::cout << "Before release " << pool->TotalMemStatistics() << std::endl;
  const auto device_addr1 = pool->AllocTensorMem(512, true);
  const auto device_addr2 = pool->AllocTensorMem(512, true);
  const auto device_addr3 = pool->AllocTensorMem(512, true);
  std::cout << "Before release " << device_addr1 << std::endl;
  std::cout << "Before release " << device_addr2 << std::endl;
  std::cout << "Before release " << device_addr3 << std::endl;
  pool->FreeTensorMem(device_addr3);
  std::cout << "After release 1 : " << device_addr1 << std::endl;
  std::cout << "After release 1 : " << device_addr2 << std::endl;
  std::cout << "After release 1 : " << device_addr3 << std::endl;
  pool->FreeTensorMem(device_addr2);
  std::cout << "After release 2 : " << device_addr1 << std::endl;
  std::cout << "After release 2 : " << device_addr2 << std::endl;
  std::cout << "After release 2 : " << device_addr3 << std::endl;
  pool->FreeTensorMem(device_addr1);
  std::cout << "After release 3 : " << device_addr1 << std::endl;
  std::cout << "After release 3 : " << device_addr2 << std::endl;
  std::cout << "After release 3 : " << device_addr3 << std::endl;
  auto release_size = pool->ReleaseFreeBlocks();
  std::cout << "Release " << release_size << "," << pool->TotalMemStatistics() << std::endl;
  delete pool;

  std::unordered_map<void *, size_t> addr_size_map;
  void *a = malloc(1024);
  std::cout << a << std::endl;
  addr_size_map.emplace(a, 1024);
  void *b = malloc(2048);
  std::cout << b << std::endl;
  addr_size_map.emplace(b, 2048);
  addr_size_map.erase(a);
  std::cout << a << std::endl;
  test();
  std::cout << "is trivial : " << (std::is_trivial_v<EventBase>) << std::endl;
  std::cout << "PlainObj is trivial : " << (std::is_trivial_v<PlainObj>) << std::endl;
  return 0;
}
