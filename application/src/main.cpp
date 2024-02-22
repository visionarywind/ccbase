#include <atomic>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

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

int main() {
  DynamicMemPoolBestFit pool;
  return 0;
}

int main2() {
  std::cout << "Application start" << std::endl;
  // ActorHashTest();
  // PriorityQueueTest();
  // TemplateTest();
  std::map<size_t, SNode *> m;
  SkipList<size_t, SNode *> s(5);

  auto insert_start = Get();
  for (size_t i = 0; i < 1000; i++) {
    m[i * 10] = new SNode();
  }
  auto insert_cost = Get() - insert_start;
  std::cout << "insert cost : " << insert_cost / 1000.0 << std::endl;

  insert_start = Get();
  for (size_t i = 0; i < 1000; i++) {
    s.insert_element(i * 10, new SNode());
  }
  insert_cost = Get() - insert_start;
  std::cout << "skip insert cost : " << insert_cost / 1000.0 << std::endl;

  auto start = Get();
  for (size_t i = 0; i < 1000; i++) {
    m.find(i);
  }
  auto cost = Get() - start;
  std::cout << "cost : " << cost * 1.0 / 1000 << std::endl;

  auto skip_start = Get();
  for (size_t i = 0; i < 1000; i++) {
    s.search_element(i);
  }
  auto skip_cost = Get() - skip_start;
  std::cout << "skip cost : " << skip_cost * 1.0 / 1000 << std::endl;

  cout << "s0 : " << (s.search_element(0) == nullptr) << endl;
  cout << "s10 : " << (s.search_element(10) == nullptr) << endl;
  cout << "s11 : " << (s.search_element(11) == nullptr) << endl;
  cout << "s100 : " << (s.search_element(100) == nullptr) << endl;

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