#include <atomic>
#include <iostream>
#include <memory>
#include <vector>
#include "inherit.h"
#include "ref.h"
#include "robin_hood.h"
#include "template.h"
#include "tools/timer.h"
#include "common/common.h"
#include "actor_hash.h"

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

int main() {
  std::cout << "Application start" << std::endl;
  ActorHashTest();
  std::cout << "Application end" << std::endl;
  return 0;
}

void DefineFunc(bool *const /*flag_ptr*/, int data) {
  std::cout << "data : " << data << std::endl;
}

void test() {
  base_test();
  int a = 1;
  void *t = &a;
  std::shared_ptr<void *> m = std::make_shared<void *>(t);
  std::cout << m << std::endl;
  m.reset();
}