//
// Created by shanfeng on 4/11/2023.
//

#ifndef CCBASE_HQUEUE_H
#define CCBASE_HQUEUE_H

template <typename T>
class HQueue;
struct Pointer {
  int32_t index = -1;
  uint32_t version = 0;
  bool operator==(const Pointer &that) const { return (index == that.index && version == that.version); }
  bool operator!=(const Pointer &that) const { return !(*this == that); }
};

template <typename T>
struct HQNode {
  std::atomic<Pointer> next;
  T *value = nullptr;
  std::atomic_bool free = {true};
};

template <typename T>
class HQueue {
 public:
  HQueue(const HQueue &) = delete;
  HQueue &operator=(const HQueue &) = delete;
  HQueue() {}
  virtual ~HQueue() {}

  bool IsInit() const { return nodes.size() != 0; }

  bool Init(int32_t sz) {
    if (IsInit() || sz <= 0) {
      return false;
    }
    for (int32_t i = 0; i < sz; i++) {
      auto node = new HQNode<T>();
      if (node == nullptr) {
        Clean();
        return false;
      }
      node->value = nullptr;
      node->free = true;
      node->next = {-1, 0};
      nodes.emplace_back(node);
    }

    // init first node as dummy head
    qhead = {0, 0};
    qtail = {0, 0};
    nodes[0]->free = false;
    queue_size = sz;
    free_index = 1;
    return true;
  }

  void Clean() {
    for (auto node : nodes) {
      delete node;
    }
    nodes.clear();
  }

  bool Enqueue(T *t) {
    HQNode<T> *node = nullptr;
    int32_t nodeIdx = free_index;
    for (; nodeIdx < queue_size; ++nodeIdx) {
      bool expected = true;
      if (nodes[nodeIdx]->free.compare_exchange_strong(expected, false)) {
        node = nodes[nodeIdx];
        free_index = nodeIdx + 1;
        break;
      }
    }
    if (node == nullptr) {
      free_index = 1;
      for (nodeIdx = 1; nodeIdx < queue_size; ++nodeIdx) {
        bool expected = true;
        if (nodes[nodeIdx]->free.compare_exchange_strong(expected, false)) {
          node = nodes[nodeIdx];
          free_index = nodeIdx + 1;
          break;
        }
      }
      if (node == nullptr) {
        return false;
      }
    }

    node->value = t;
    node->next = {-1, 0};

    while (true) {
      Pointer tail = qtail;
      if (tail.index == -1) {
        continue;
      }
      Pointer next = nodes[tail.index]->next;

      if (tail != this->qtail) {
        continue;
      }

      if (next.index != -1) {
        this->qtail.compare_exchange_strong(tail, {next.index, tail.version + 1});
        continue;
      }

      if (nodes[tail.index]->next.compare_exchange_strong(next, {nodeIdx, next.version + 1})) {
        this->qtail.compare_exchange_strong(tail, {nodeIdx, tail.version + 1});
        break;
      }
    }

    return true;
  }

  T *Dequeue() {
    while (true) {
      T *ret = nullptr;
      Pointer head = qhead;
      Pointer tail = qtail;
      if (head.index == -1) {
        continue;
      }
      Pointer next = nodes[head.index]->next;

      if (head != this->qhead) {
        continue;
      }

      if (head.index == tail.index) {
        if (next.index == -1) {
          return nullptr;
        }
        this->qtail.compare_exchange_strong(tail, {next.index, tail.version + 1});
      } else {
        if (next.index == -1) {
          continue;
        }
        ret = nodes[next.index]->value;
        if (this->qhead.compare_exchange_strong(head, {next.index, head.version + 1})) {
          // free head
          nodes[head.index]->free = true;
          return ret;
        }
      }
    }
  }

  bool Empty() {
    Pointer head = qhead;
    Pointer tail = qtail;
    if (head.index < 0) {
      return false;
    }
    Pointer next = nodes[head.index]->next;

    if (head == this->qhead && head.index == tail.index && next.index == -1) {
      return true;
    }

    return false;
  }

 private:
  std::atomic<Pointer> qhead;
  std::atomic<Pointer> qtail;
  std::vector<HQNode<T> *> nodes;
  int32_t queue_size{};
  std::atomic<int32_t> free_index;
};

#endif //CCBASE_HQUEUE_H
