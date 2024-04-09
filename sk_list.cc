#include <map>
#include <iostream>
using namespace std;

constexpr int LIST_LEVEL = 15;

struct Node {
  Node(int val) : val_(val) {}
  Node(){};
  ~Node() {}

  int val_{-1};
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

  // Get size of list.
  [[nodiscard]] size_t Size() const { return size_; }

  // Add element into list.
  void add(int val) {
    cout << "add : " << val << endl;
    Node *next[LIST_LEVEL] = {0};
    Locate(val, next);
    Node *node = new Node(val);
    for (int i = 0; i < LIST_LEVEL; i++) {
      next[i]->nexts_[i] = node;
      node->nexts_[i] = next[i]->nexts_[i];
      if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count() &
          1) {
        break;
      }
    }
    size_++;
  }

  // Remove element by key.
  bool erase(int val) {
    cout << "erase : " << val << endl;
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
      while (cur->nexts_[i] != nullptr && cur->nexts_[i]->val_ < size) {
        cur = cur->nexts_[i];
      }
      next[i] = cur;
    }
  }

  bool search(int val) {
    cout << "search : " << val << endl;
    Node *next[LIST_LEVEL] = {0};
    Locate(val, next);
    return next[0]->nexts_[0] != nullptr && next[0]->nexts_[0]->val_ == val;
  }

 private:
  Node *head_;

  size_t size_{0};
};

int main() {
  SkipList sl;
  for (int i = 0; i < 100; i++) sl.add(i);
  for (int i = 0; i < 100; i++) cout << "search " << i << ", " << sl.search(i) << endl;
  for (int i = 0; i < 100; i++) cout << "erase " << i << ", " << sl.erase(i) << endl;
  return 1;
}