#pragma once

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <random>

using namespace std;

using DeviceEventPtr = void *;

constexpr int32_t LIST_LEVEL = 15;

template <typename K, typename V>
struct Node {
  Node(K key, V value) : key_(key), value_(value) {}
  Node(){};
  ~Node() {}

  K key_;
  V value_;
  Node *nexts_[LIST_LEVEL]{nullptr};
};

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
  SortedList(Comparator comparator = Comparator{}) : head_(new Node<K, V>()), last_(head_), comparator_(comparator) {}
  ~SortedList();
  // Disable copy and assignment constructor.
  SortedList(const SortedList &other) = delete;
  SortedList &operator=(SortedList const &) = delete;

  // Get largest task id on stream.
  K LargestTaskIdOnStream() const { return last_ == head_ ? -1L : last_->key_; }
  // Get smallest task id on stream.
  K SmallestTaskIdOnStream() const { return head_->nexts_[0] ? head_->nexts_[0]->key_ : -1L; }
  // Insert element into list.
  void Insert(K id, V event) { Add(id, event); }
  // Traverse elements in list.
  void Traverse(std::function<void(K, V &)> func);
  // Remove range elements that less than id.
  bool RemoveLess(K id);
  // Remove range elements that not greater than id.
  bool RemoveLowerBound(K key);
  // Get size of list.
  [[nodiscard]] size_t Size() const { return size_; }
  // Print elements in list.
  void Print();

  // Get element is list.
  V Get(K key);
  // Add element into list.
  void Add(K key, V event);
  // Remove element by key.
  bool Remove(K key);

 private:
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
void SortedList<K, V, Comparator>::Add(K key, V event) {
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
}

template <typename K, typename V, class Comparator>
bool SortedList<K, V, Comparator>::Remove(K key) {
  Node<K, V> *next[LIST_LEVEL];
  Locate(key, next);
  Node<K, V> *node = next[0]->nexts_[0];
  if (node == nullptr || comparator_(node->key_, key) != 0) {
    return false;
  }
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
void SortedList<K, V, Comparator>::Locate(K key, Node<K, V> *next[]) {
  Node<K, V> *cur = head_;
  for (int i = LIST_LEVEL - 1; i >= 0; i--) {
    while (cur->nexts_[i] != nullptr && comparator_(cur->nexts_[i]->key_, key) == -1) {
      cur = cur->nexts_[i];
    }
    next[i] = cur;
  }
}

template <typename K, typename V, class Comparator>
void SortedList<K, V, Comparator>::LocateLowerBound(K key, Node<K, V> *next[]) {
  Node<K, V> *cur = head_;
  for (int i = LIST_LEVEL - 1; i >= 0; i--) {
    while (cur->nexts_[i] != nullptr && comparator_(cur->nexts_[i]->key_, key) != 1) {
      cur = cur->nexts_[i];
    }
    next[i] = cur;
  }
}