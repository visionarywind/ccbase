#pragma once

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <random>

using namespace std;

constexpr int32_t LIST_LEVEL = 15;

template <typename K>
struct DefaultComparator : std::binary_function<K, K, int> {
  int operator()(K a, K b) const {
    if (a < b) {
      return -1;
    } else if (a > b) {
      return 1;
    } else {
      return 0;
    }
  }
};

// Sorted list inspired from skip list.
template <typename K, typename V, class Comparator = DefaultComparator<K>>
class SortedList {
 public:
  template <typename K, typename V>
  struct Node {
    Node(K key, V value) : key_(key), value_(value) {}
    Node(){};
    ~Node() {}

    K key_;
    V value_;
    Node *nexts_[LIST_LEVEL]{nullptr};
  };

 public:
  SortedList(Comparator comparator = Comparator{}) : head_(new Node<K, V>()), last_(head_), comparator_(comparator) {}
  ~SortedList();
  // Disable copy and assignment constructor.
  SortedList(const SortedList &other) = delete;
  SortedList &operator=(SortedList const &) = delete;

  // Get element is list, unsafe.
  V Get(K key);
  // Add element into list.
  void Insert(K key, V value);
  // Remove element by key and value.
  bool Remove(K key, V value);

  // Get largest.
  Node *Largest() const { return last_ == head_ ? nullptr : last_; }
  // Get smallest.
  Node *Smallest() const { return head_->nexts_[0]; }

  // Traverse elements in list.
  void Traverse(std::function<void(K, V &)> func);
  // Remove range elements that less than id.
  bool RemoveLess(K id);
  // Remove range elements that not greater than id.
  bool RemoveLowerBound(K key);
  bool RemoveNode(Node<K, V> *node, Node<K, V> *next[]);
  // Get size of list.
  [[nodiscard]] size_t Size() const { return size_; }
  // Print elements in list.
  void Print();

 public:
  // Locate position that less than key.
  void Locate(K key, Node<K, V> *next[]);
  // Locate position that not greater than key.
  void LocateLowerBound(K key, Node<K, V> *next[]);

 private:
  Node<K, V> *head_;
  Node<K, V> *last_;
  Comparator comparator_;
  size_t size_{0};
};

template <typename K, typename V, class Comparator>
SortedList<K, V, Comparator>::~SortedList() {
  Node<K, V> *begin = head_->nexts_[0];
  while (begin != nullptr) {
    Node<K, V> *to_delete = begin;
    begin = begin->nexts_[0];
    delete to_delete;
  }
  delete head_;
}

template <typename K, typename V, class Comparator>
void SortedList<K, V, Comparator>::Traverse(std::function<void(K, V &)> func) {
  Node<K, V> *begin = head_->nexts_[0];
  while (begin != nullptr) {
    func(begin->key_, begin->value_);
    begin = begin->nexts_[0];
  }
}

template <typename K, typename V, class Comparator>
bool SortedList<K, V, Comparator>::RemoveLess(K key) {
  Node<K, V> *next[LIST_LEVEL];
  Locate(key, next);
  Node<K, V> *begin = head_->nexts_[0];
  Node<K, V> *end = next[0]->nexts_[0];
  if (end == nullptr) {
    // Need clear all nodes.
    while (begin != end) {
      Node<K, V> *to_delete = begin;
      begin = begin->nexts_[0];
      delete to_delete;
    }
    for (int i = LIST_LEVEL - 1; i >= 0; i--) {
      head_->nexts_[i] = nullptr;
      last_ = head_;
    }
    size_ = 0;
    return false;
  }

  for (int i = LIST_LEVEL - 1; i >= 0; i--) {
    head_->nexts_[i] = next[i]->nexts_[i];
  }
  while (begin != end) {
    Node<K, V> *to_delete = begin;
    begin = begin->nexts_[0];
    size_--;
    delete to_delete;
  }
  return true;
}

template <typename K, typename V, class Comparator>
bool SortedList<K, V, Comparator>::RemoveLowerBound(K key) {
  // auto start = GetCurrentTime();
  Node<K, V> *next[LIST_LEVEL];
  LocateLowerBound(key, next);
  Node<K, V> *begin = head_->nexts_[0];
  Node<K, V> *end = next[0]->nexts_[0];
  if (end == nullptr) {
    // Need clear all nodes.
    while (begin != end) {
      Node<K, V> *to_delete = begin;
      begin = begin->nexts_[0];
      delete to_delete;
    }
    for (int i = LIST_LEVEL - 1; i >= 0; i--) {
      head_->nexts_[i] = nullptr;
      last_ = head_;
    }
    size_ = 0;
    return false;
  }

  for (int i = LIST_LEVEL - 1; i >= 0; i--) {
    head_->nexts_[i] = next[i]->nexts_[i];
  }
  while (begin != end) {
    Node<K, V> *to_delete = begin;
    begin = begin->nexts_[0];
    size_--;
    delete to_delete;
  }
  // auto cost = GetCurrentTime() - start;
  // MS_LOG(ERROR) << "list remove cost : " << cost / 1000.0 << "us.";
  return true;
}

template <typename K, typename V, class Comparator>
void SortedList<K, V, Comparator>::Print() {
  Traverse([](K key, V &value) -> void {});
}

template <typename K, typename V, class Comparator>
V SortedList<K, V, Comparator>::Get(K key) {
  Node<K, V> *next[LIST_LEVEL];
  Locate(key, next);
  if (next[0]->nexts_[0] != nullptr && comparator_(next[0]->nexts_[0]->key_, key) == 0) {
    return next[0]->nexts_[0]->value_;
  } else {
    return nullptr;
  }
}

template <typename K, typename V, class Comparator>
void SortedList<K, V, Comparator>::Insert(K key, V event) {
  // std::cout << "enter add" << std::endl;
  // auto start = GetCurrentTime();
  Node<K, V> *next[LIST_LEVEL];
  Locate(key, next);
  Node<K, V> *node = new Node<K, V>(key, event);
  if (next[0]->nexts_[0] == nullptr) {
    last_ = node;
  }
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
  // std::cout << "exit add, size_ : " << size_ << std::endl;
}

template <typename K, typename V, class Comparator>
bool SortedList<K, V, Comparator>::RemoveNode(Node<K, V> *node, Node<K, V> *next[]) {
  for (int i = 0; i < LIST_LEVEL && next[i]->nexts_[i] == node; i++) {
    next[i]->nexts_[i] = next[i]->nexts_[i]->nexts_[i];
  }
  if (next[0]->nexts_[0] == nullptr) {
    last_ = next[0];
  }
  delete node;
  size_--;
  return true;
}

template <typename K, typename V, class Comparator>
bool SortedList<K, V, Comparator>::Remove(K key, V value) {
  // std::cout << "enter remove" << std::endl;
  Node<K, V> *next[LIST_LEVEL];
  Locate(key, next);
  Node<K, V> *node = next[0]->nexts_[0];
  bool found = false;
  while (node != nullptr && comparator_(node->key_, key) == 0) {
    if (node->value_ == value) {
      found = true;
      break;
    }
    node = node->nexts_[0];
  }
  if (found) {
    RemoveNode(node, next);
  }
  // std::cout << "exit remove, element size : " << size_ << std::endl;
  return found;
}

template <typename K, typename V, class Comparator>
void SortedList<K, V, Comparator>::Locate(K key, Node<K, V> *next[]) {
  // cout << "enter locate" << endl;
  Node<K, V> *cur = head_;
  if (cur == nullptr) {
    cout << "exception" << endl;
  }
  for (int i = LIST_LEVEL - 1; i >= 0; i--) {
    // if (size_ >= 2400) {
    //   cout << "1" << endl;
    //   cout << "1 : " << i << ", cur : " << cur << ", next : " << cur->nexts_[i] << endl;
    //   if (cur->nexts_[i]) cout << "key : " << cur->nexts_[i]->key_ << endl;
    // }
    while (cur->nexts_[i] != nullptr && comparator_(cur->nexts_[i]->key_, key) == -1) {
      // if (size_ > 2400) {
      //   cout << "2 : " << i << ", cur->nexts_[i] : " << cur->nexts_[i] << endl;
      // }
      cur = cur->nexts_[i];
      if (cur == nullptr) {
        cout << "exception2" << endl;
      }
    }
    // if (size_ > 2400) {
    //   cout << "3 : " << i << endl;
    // }
    next[i] = cur;
  }
}

template <typename K, typename V, class Comparator>
void SortedList<K, V, Comparator>::LocateLowerBound(K key, Node<K, V> *next[]) {
  Node<K, V> *cur = head_;
  for (int i = LIST_LEVEL - 1; i >= 0; i--) {
    while (cur->nexts_[i] != nullptr) {
      auto ret = comparator_(cur->nexts_[i]->key_, key);
      if (ret == -1) {
        cur = cur->nexts_[i];
      } else if (ret == 0) {
        cur = cur->nexts_[i];
        break;
      } else {
        break;
      }
    }
  }
}