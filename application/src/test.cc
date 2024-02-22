#include <atomic>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "skiplist.h"

using namespace std;

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
  s.delete_element(0);
  s.delete_element(10);

  cout << "s0 : " << (s.search_element(0) == nullptr) << endl;
  cout << "s10 : " << (s.search_element(10) == nullptr) << endl;
  cout << "s11 : " << (s.search_element(11) == nullptr) << endl;
  cout << "s100 : " << (s.search_element(100) == nullptr) << endl;

  // s.display_list();

  return 0;

  std::cout << "Application end" << std::endl;
  return 0;
}