#include <atomic>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>
#include <unordered_map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "actor_hash.h"
#include "common/common.h"
#include "inherit.h"
#include "ref.h"
#include "robin_hood.h"
#include "sorted_vector.h"
#include "template.h"
#include "tools/timer.h"
#include "skiplist.h"
#include "mem_dynamic_allocator.h"

using namespace std;

void test();

struct IdGenerator {
  IdGenerator() {
    static std::atomic<int> id_gen_;
    id_ = id_gen_++;
  }
  int id_;
};

void DefineFunc(bool *const flag_ptr, int data);

enum class EndlessType : int {
  kFirst,
  kSecond,
  kThird,

  kEnd = kThird
};

void SimpleTest() {
  DefineFunc(nullptr, 100);

  std::cout << static_cast<int>(EndlessType::kFirst) << std::endl;
  std::cout << static_cast<int>(EndlessType::kThird) << std::endl;

  // new feature for c++17 ?
  int data = 100;
  if (auto i = false ? &data : nullptr) {
    std::cout << " i = " << i << std::endl;
  }

  IdGenerator id1;
  IdGenerator id2;
  std::cout << "id1 " << id1.id_ << std::endl;
  std::cout << "id2 " << id2.id_ << std::endl;

  // ReferTest();
  TemplateTest();

  std::cout << endsWith("asadad-/Mul-op123", "Mul-op123") << std::endl;
  std::cout << endsWith("asadad-/Mul-op123", "Mul-op13") << std::endl;
  std::cout << endsWith("asdas", "as") << std::endl;
}

constexpr int kTestCount = 1 << 18;

int RobinHashTest() {
  robin_hood::unordered_map<std::string, std::string> map;
  {
    TimerClock clock("insertion");
    for (int i = 0; i != kTestCount; i++) {
      auto key = "kernel_graph_" + std::to_string(i);
      map.emplace(key, std::to_string(i));
    }
  }
  int hits = 0;
  {
    TimerClock clock("get");
    for (int i = 0; i != kTestCount; i++) {
      auto key = "kernel_graph_" + std::to_string(i);
      if (map.count(key) != 0) {
        map.at(key);
        hits++;
      }
    }
  }

  return hits;
}

int StlHashTest() {
  std::unordered_map<std::string, std::string> map;
  {
    TimerClock clock("insertion");
    for (int i = 0; i != kTestCount; i++) {
      auto key = "kernel_graph_" + std::to_string(i);
      map.emplace(key, std::to_string(i));
    }
  }
  int hits = 0;
  {
    TimerClock clock("get");
    for (int i = 0; i != kTestCount; i++) {
      auto key = "kernel_graph_" + std::to_string(i);
      if (map.count(key) != 0) {
        map.at(key);
        hits++;
      }
    }
  }

  return hits;
}

int FlatHashTest() {
  robin_hood::unordered_flat_map<std::string, std::string> map;
  {
    TimerClock clock("insertion");
    for (int i = 0; i != kTestCount; i++) {
      auto key = "kernel_graph_" + std::to_string(i);
      map.emplace(key, std::to_string(i));
    }
  }
  int hits = 0;
  {
    TimerClock clock("get");
    for (int i = 0; i != kTestCount; i++) {
      auto key = "kernel_graph_" + std::to_string(i);
      if (map.count(key) != 0) {
        map.at(key);
        hits++;
      }
    }
  }

  return hits;
}

void HashTest() {
  std::cout << "Robin hash : " << RobinHashTest() << std::endl;
  std::cout << "Flat hash : " << FlatHashTest() << std::endl;
  std::cout << "Stl hash : " << StlHashTest() << std::endl;
}

inline int64_t Get() {
  auto now_time = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(now_time.time_since_epoch()).count();
}

struct SNode {
  SNode() = default;
  SNode(int a) { this->a = a; }
  ~SNode() = default;
  int a;
  SNode *prev;
  SNode *next;
};

// int main() {
//   DynamicMemPoolBestFit pool;
//   return 0;
// }

unsigned char parse_hex(char c) {
  if ('0' <= c && c <= '9') return c - '0';
  if ('A' <= c && c <= 'Z') return c - 'A' + 10;
  if ('a' <= c && c <= 'z') return c - 'a' + 10;
}

std::vector<unsigned char> parse_string(const std::string &s) {
  std::vector<unsigned char> result(s.size() / 2);

  for (std::size_t i = 0; i != s.size() / 2; ++i) result[i] = 16 * parse_hex(s[2 * i]) + parse_hex(s[2 * i + 1]);

  return result;
}

string ToHex(char data) {
  static char const hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  string s("");
  s += hex_chars[(data & 0xF0) >> 4];
  cout << "to : " << s << endl;
  s += hex_chars[(data & 0x0F) >> 0];
  cout << "to : " << s << endl;
  return s;
}

int main() {
  std::cout << "Application start2" << std::endl;
  // json data = {{"pi", 3.141},
  //              {"happy", true},
  //              {"name", "Niels"},
  //              {"nothing", nullptr},
  //              {"answer", {{"everything", "0"}}},
  //              {"list", {1, 0, 2}},
  //              {"object", {{"currency", "USD"}, {"value", 42.99}}}};
  // cout << "data : " << data.dump() << endl;
  // json out_data;
  // for (int i = 0; i < 100000; i++) {
  //   out_data[std::to_string(i)] = "I am a big string_" + std::to_string(i);
  // }
  // cout << "out_data : " << out_data.size() << endl;
  json new_data = json::parse("12312312");
  cout << "new_data : size : " << new_data.size() << endl;

  // string s("zzhahaha");
  // stringstream ss;
  // const char *base = reinterpret_cast<const char *>(s.c_str());
  // for (int i = 0; i < s.length(); i++) {
  //   // cout << std::hex << ToHex(*(base + i)) << endl;
  //   ss << std::hex << (int)(*(base + i)) << " ";
  // }
  // cout << ss.str() << endl;

  // ActorHashTest();
  // PriorityQueueTest();
  // TemplateTest();
  // std::map<size_t, SNode *> m;
  // SkipList<size_t, SNode *> s(5);

  // auto insert_start = Get();
  // for (size_t i = 0; i < 1000; i++) {
  //   m[i * 10] = new SNode();
  // }
  // auto insert_cost = Get() - insert_start;
  // std::cout << "insert cost : " << insert_cost / 1000.0 << std::endl;

  // insert_start = Get();
  // for (size_t i = 0; i < 1000; i++) {
  //   s.insert_element(i * 10, new SNode());
  // }
  // insert_cost = Get() - insert_start;
  // std::cout << "skip insert cost : " << insert_cost / 1000.0 << std::endl;

  // auto start = Get();
  // for (size_t i = 0; i < 1000; i++) {
  //   m.find(i);
  // }
  // auto cost = Get() - start;
  // std::cout << "cost : " << cost * 1.0 / 1000 << std::endl;

  // auto skip_start = Get();
  // for (size_t i = 0; i < 1000; i++) {
  //   s.search_element(i);
  // }
  // auto skip_cost = Get() - skip_start;
  // std::cout << "skip cost : " << skip_cost * 1.0 / 1000 << std::endl;

  // cout << "s0 : " << (s.search_element(0) == nullptr) << endl;
  // cout << "s10 : " << (s.search_element(10) == nullptr) << endl;
  // cout << "s11 : " << (s.search_element(11) == nullptr) << endl;
  // cout << "s100 : " << (s.search_element(100) == nullptr) << endl;

  // s.display_list();

  return 0;

  std::cout << "Application end" << std::endl;
  return 0;
}

void DefineFunc(bool *const /*flag_ptr*/, int data) { std::cout << "data : " << data << std::endl; }

void test() {
  base_test();
  int a = 1;
  void *t = &a;
  std::shared_ptr<void *> m = std::make_shared<void *>(t);
  std::cout << m << std::endl;
  m.reset();
}