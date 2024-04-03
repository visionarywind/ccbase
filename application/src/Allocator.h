#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

// #include "sorted_list.h"
// Sorted list inspired from skip list.

constexpr int32_t LIST_LEVEL = 15;

struct MemBlock {
  MemBlock(void *addr, size_t size) : addr_(addr), size_(size) {}
  void *addr_;
  size_t size_;
};

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

struct Node {
  Node(BlockRawPtr block) : block_(block) {}
  Node(){};
  ~Node() {}

  BlockRawPtr block_{nullptr};
  Node *nexts_[LIST_LEVEL]{nullptr};
};

class SkipList {
 public:
  SkipList() : head_(new Node()), last_(head_) {}
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

  // Get largest task id on stream.
  BlockRawPtr LargestTaskIdOnStream() const { return last_ == head_ ? nullptr : last_->block_; }

  // Get smallest task id on stream.
  BlockRawPtr SmallestTaskIdOnStream() const { return head_->nexts_[0] ? head_->nexts_[0]->block_ : nullptr; }
  // Traverse elements in list.
  void Traverse(std::function<void(BlockRawPtr)> func) {
    Node *begin = head_->nexts_[0];
    while (begin != nullptr) {
      func(begin->block_);
      begin = begin->nexts_[0];
    }
  }

  bool RemoveNode(Node *node, Node *next[]) {
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

  // Get size of list.
  [[nodiscard]] size_t Size() const { return size_; }
  // Print elements in list.
  void Print() {
    Traverse([](BlockRawPtr block) -> void {
      std::cout << "block : " << block << ", size : " << block->size_ << ", addr : " << block->addr_ << std::endl;
    });
  }

  // Add element into list.
  void Insert(BlockRawPtr block) {
    // std::cout << "enter add" << std::endl;
    // auto start = GetCurrentTime();
    Node *next[LIST_LEVEL];
    Locate(block->size_, next);
    Node *node = new Node(block);
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
  // Remove element by key.
  bool Remove(BlockRawPtr block) {
    std::cout << "enter remove" << std::endl;
    Node *next[LIST_LEVEL];
    Locate(block->size_, next);
    Node *node = next[0]->nexts_[0];
    bool found = false;
    while (node != nullptr && node->block_->size_ == block->size_) {
      if (node->block_ == block) {
        found = true;
        break;
      }
      node = node->nexts_[0];
      if (node->block_ == nullptr) {
        std::cout << "unknown branch" << std::endl;
      }
    }
    if (found) {
      RemoveNode(node, next);
    }
    std::cout << "exit remove, element size : " << size_ << std::endl;
    return found;
  }

 public:
  // Locate position that less than key.
  void Locate(size_t size, Node *next[]) {
    Node *cur = head_;
    if (cur == nullptr) {
      std::cout << "exception" << std::endl;
    }
    for (int i = LIST_LEVEL - 1; i >= 0; i--) {
      // if (size_ >= 2400) {
      //   cout << "1" << endl;
      //   cout << "1 : " << i << ", cur : " << cur << ", next : " << cur->nexts_[i] << endl;
      //   if (cur->nexts_[i]) cout << "key : " << cur->nexts_[i]->key_ << endl;
      // }
      while (cur->nexts_[i] != nullptr && cur->nexts_[i]->block_->size_ < size) {
        // if (size_ > 2400) {
        //   cout << "2 : " << i << ", cur->nexts_[i] : " << cur->nexts_[i] << endl;
        // }
        cur = cur->nexts_[i];
        if (cur == nullptr) {
          std::cout << "exception2" << std::endl;
        }
      }
      // if (size_ > 2400) {
      //   cout << "3 : " << i << endl;
      // }
      next[i] = cur;
    }
  }

 private:
  Node *head_;
  Node *last_;

  size_t size_{0};
};

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

class Allocator {
 public:
  virtual void *Alloc(size_t size, uint32_t stream_id = 0) = 0;
  virtual bool Free(void *addr) = 0;

  virtual void *MemAlloc(size_t size, uint32_t stream_id = 0) = 0;
  virtual bool MemFree(void *addr) = 0;
};

class MallocAllocator : public Allocator {
 protected:
  void *MemAlloc(size_t size, uint32_t stream_id = 0) override {
    void *addr = malloc(size);
    auto mem_block = new MemBlock(addr, size);
    allocated_blocks_[addr] = mem_block;
    return addr;
  }

  bool MemFree(void *addr) override {
    auto iter = allocated_blocks_.find(addr);
    if (iter != allocated_blocks_.end()) {
      auto mem_block = iter->second;
      allocated_blocks_.erase(iter);
      delete mem_block;
      return true;
    }
    return false;
  }

 protected:
  std::unordered_map<void *, MemBlock *> allocated_blocks_;
};
struct Comparator {
  bool operator()(const void *addr1, const void *addr2) const { return addr1 < addr2; }
};

class DefaultAllocator : public MallocAllocator {
 public:
  size_t MIN_SPLIT_SIZE = 512;

  void *Alloc(size_t size, uint32_t stream_id = 0) override;

  bool Free(void *addr) override;

 private:
  std::vector<std::set<BlockRawPtr, BlockComparator>> free_blocks_{decltype(free_blocks_)(128)};
  std::unordered_map<void *, BlockRawPtr> total_block_;
  std::mutex mutex_;
};

class SkipListAllocator : public MallocAllocator {
 public:
  size_t MIN_SPLIT_SIZE = 512;

  void *Alloc(size_t size, uint32_t stream_id = 0) override;

  bool Free(void *addr) override;

 private:
  std::vector<SkipList> free_blocks_{decltype(free_blocks_)(128)};
  std::unordered_map<void *, BlockRawPtr> total_block_;
  std::mutex mutex_;
};