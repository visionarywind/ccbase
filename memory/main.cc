#include "memory_pool.h"

#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
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

int main() {
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
  return 0;
}