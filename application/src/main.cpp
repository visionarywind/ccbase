#include <atomic>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
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
#include "sorted_list.h"
#include "mem_dynamic_allocator.h"
#include "layer.h"

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

double TimeSinceEpoch() {
  auto now = std::chrono::steady_clock::now();
  auto d = now.time_since_epoch();
  return d.count();
}

struct Block {
  Block(void *addr, size_t size, int status) : addr_(addr), size_(size), status_(status) {}
  void *addr_;
  size_t size_;
  int status_;  // 0 : free, 1 : used, 2 : eager free.

  int stream_id_{0};
  struct Block *prev_{nullptr};
  struct Block *next_{nullptr};

  void Print() {
    std::cout << "Block[" << addr_ << "] size_ : " << size_ << ", status_ : " << status_
              << ", prev_ : " << (prev_ ? prev_->addr_ : 0) << ", next_ : " << (next_ ? next_->addr_ : 0) << std::endl;
  }
};
using BlockRawPtr = struct Block *;

class BlockComparator {
 public:
  bool operator()(const BlockRawPtr &left, const BlockRawPtr &right) const {
    // stream id is not used currently
    // if (left->stream_id_ != right->stream_id_) {
    //   return left->stream_id_ < right->stream_id_;
    // }
    return (left->size_ != right->size_) ? left->size_ < right->size_ : left->addr_ < right->addr_;
  }

  // bool operator()(const size_t size, const BlockRawPtr &block) const { return size < block->size_; }
  // bool operator()(const BlockRawPtr &block, const size_t size) const { return block->size_ < size; }
};

struct BlockIntComparator {
  int operator()(const BlockRawPtr &left, const BlockRawPtr &right) const {
    if (left->size_ < right->size_) {
      return -1;
    } else if (left->size_ > right->size_) {
      return 1;
    } else {
      if (left->addr_ < right->addr_) {
        return -1;
      } else if (left->addr_ > right->addr_) {
        return 1;
      } else {
        return 0;
      }
    }
  }
};

int main() {
  // cout << LayerAdd(1, 1) << endl;
  // cout << LayerAdd(1, 1) << endl;
  int count = 1000000;
  std::set<BlockRawPtr, BlockComparator> set_base;
  SortedList<size_t, BlockRawPtr> sort_list;
  vector<BlockRawPtr> inputs;
  for (int i = 0; i < count; i++) {
    inputs.emplace_back(new Block(malloc(i), i, 0));
  }
  cout << "start test" << endl;
  // Alloc
  int64_t cost_in_double = 0L;
  for (int i = 0; i < count; i++) {
    auto start_in_double = Get();
    set_base.emplace(inputs[i]);
    cost_in_double += Get() - start_in_double;
  }
  cout << "emplace cost in set : " << cost_in_double * 1.0 / 1000 / count << ".us" << endl;
  // Free
  cost_in_double = 0;
  for (int i = 0; i < count; i++) {
    auto start_in_double = Get();
    auto iter = set_base.find(inputs[i]);
    set_base.erase(iter);
    cost_in_double += Get() - start_in_double;
  }
  cout << "erase cost in set : " << cost_in_double * 1.0 / 1000 / count << ".us" << endl;
  cout << "After free size : " << set_base.size() << endl;

  // Alloc
  int64_t cost = 0L;
  for (int i = 0; i < count; i++) {
    auto start = Get();
    sort_list.Add(inputs[i]->size_, inputs[i]);
    cost += Get() - start;
  }
  cout << "sorted_list add cost : " << cost * 1.0 / 1000 / count << ".us" << endl;
  cost = 0L;
  // Free

  for (int i = 0; i < count; i++) {
    auto start = Get();
    Node<size_t, BlockRawPtr> *next[LIST_LEVEL];
    sort_list.Locate(inputs[i]->size_, next);
    auto node = next[0]->nexts_[0];
    sort_list.RemoveNode(node, next);
    cost += Get() - start;
  }
  cout << "sorted_list remove cost : " << cost * 1.0 / 1000 / count << ".us" << endl;
  cout << "After remove size : " << sort_list.Size() << endl;
  return 0;
}

int skiplist_test() {
  std::cout << "Application start" << std::endl;
  // ActorHashTest();
  // PriorityQueueTest();
  // TemplateTest();
  std::map<size_t, SNode *> m;
  sk::SkipList<size_t, SNode *> s(5);

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
  // std::cout << "Application end" << std::endl;
  // return 0;
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