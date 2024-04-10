#include <chrono>
#include <map>
#include <iostream>
using namespace std;

constexpr int LIST_LEVEL = 15;

struct Node {
  Node(int val) : val_(val) {}
  Node(){};
  ~Node() {}

  int val_{INT_MAX};
  Node *nexts_[LIST_LEVEL]{nullptr};
};

class SkipList {
 public:
  SkipList() : head_(new Node()) {}
  ~SkipList() {
    Node *begin = head_->nexts_[0];
    while (begin != nullptr) {
      Node *to_delete = begin;
      begin = begin->nexts_[0];
      delete to_delete;
    }
    delete head_;
  }
  // Disable copy and assignment constructor.
  SkipList(const SkipList &other) = delete;
  SkipList &operator=(SkipList const &) = delete;

  bool RemoveNode(Node *node, Node *next[]) {
    for (int i = 0; i < LIST_LEVEL && next[i]->nexts_[i] == node; i++) {
      next[i]->nexts_[i] = next[i]->nexts_[i]->nexts_[i];
    }
    delete node;
    size_--;
    return true;
  }

  void Traverse(std::function<void(int)> func) {
    Node *begin = head_->nexts_[0];
    while (begin != nullptr) {
      func(begin->val_);
      begin = begin->nexts_[0];
    }
  }

  void Print() {
    Traverse([](int val) -> void { std::cout << "val : " << val << std::endl; });
  }
  // Get size of list.
  [[nodiscard]] size_t Size() const { return size_; }

  // Add element into list.
  void add(int val) {
    // cout << "add : " << val << endl;
    Node *next[LIST_LEVEL] = {0};
    Locate(val, next);
    Node *node = new Node(val);
    for (int i = 0; i < LIST_LEVEL; i++) {
      node->nexts_[i] = next[i]->nexts_[i];
      next[i]->nexts_[i] = node;
      if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count() &
          1) {
        break;
      }
    }
    size_++;
    // Print();
  }

  // Remove element by key.
  bool erase(int val) {
    // cout << "erase : " << val << endl;
    Node *next[LIST_LEVEL] = {0};
    Locate(val, next);
    Node *node = next[0]->nexts_[0];
    bool found = false;
    while (node != nullptr && node->val_ == val) {
      auto next_node = node->nexts_[0];
      RemoveNode(node, next);
      found = true;
      node = next_node;
    }
    return found;
  }

 public:
  // Locate position that less than key.
  void Locate(int size, Node *next[]) {
    Node *cur = head_;
    for (int i = LIST_LEVEL - 1; i >= 0; i--) {
      while (cur->nexts_[i] != nullptr && cur->nexts_[i]->val_ < size) cur = cur->nexts_[i];
      next[i] = cur;
    }
  }

  bool search(int val) {
    // cout << "search : " << val << endl;
    Node *next[LIST_LEVEL] = {0};
    Locate(val, next);
    return next[0]->nexts_[0] != nullptr && next[0]->nexts_[0]->val_ == val;
  }

 private:
  Node *head_;

  size_t size_{0};
};

inline int64_t Get() {
  auto now_time = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(now_time.time_since_epoch()).count();
}

int main() {
  SkipList sl;
  int count = 1000000;
  auto start = Get();
  for (int i = 0; i < count; i++) sl.add(i);
  // for (int i = 0; i < count; i++) cout << "search " << i << ", " << sl.search(i) << endl;
  for (int i = 0; i < count; i++) sl.erase(i);
  auto cost = Get() - start;
  cout << "sl cost : " << cost * 1.0 / 1000 / 1000 << "ms." << endl;
  cout << sl.Size() << endl;

  map<int, int> m;
  start = Get();
  for (int i = 0; i < count; i++) m.emplace(i, i);
  // for (int i = 0; i < count; i++) cout << "search " << i << ", " << sl.search(i) << endl;
  for (int i = 0; i < count; i++) m.erase(i);
  cost = Get() - start;
  cout << "map cost : " << cost * 1.0 / 1000 / 1000 << "ms." << endl;
  cout << m.size() << endl;

  // for (int i = 0; i < count; i++) cout << "search again " << i << ", " << sl.search(i) << endl;
  return 1;
}