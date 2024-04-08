#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>
#include <vector>

#include "chef_skiplist.hpp"

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
    std::cout
      << "Block[" << addr_ << "] size_ : " << size_ << ", status_ : "
      << status_
      // << ", prev_ : " << (prev_ ? prev_->addr_ : 0) << ", next_ : " << (next_ ? next_->addr_ : 0) << std::endl;
      << ", prev_ : " << prev_ << ", next_ : " << next_ << std::endl;
  }
};
using BlockRawPtr = struct Block *;

struct Node {
  Node(BlockRawPtr block) : block_(block) {}
  Node(){};
  ~Node() {}

  size_t size() { return block_->size_; }
  BlockRawPtr block_{nullptr};
  Node *nexts_[LIST_LEVEL]{nullptr};
};

class SkipList {
 public:
  SkipList() : head_(new Node()) {}
  ~SkipList() {
    std::cout << "size : " << Size() << std::endl;
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

  void Dump() {
    Traverse([](BlockRawPtr ptr) -> void {
      std::cout << "ptr : " << ptr->addr_ << ", size : " << ptr->size_ << ", status : " << ptr->status_ << std::endl;
    });
  }

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
    // std::cout << "delete node : " << node << std::endl;
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
    // std::cout << "Insert block : " << block << ", size : " << block->size_ << std::endl;
    Node *next[LIST_LEVEL] = {0};
    Locate(block->size_, next);
    Node *node = new Node(block);
    // std::cout << "new node : " << node << std::endl;
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
    Node *next[LIST_LEVEL] = {0};
    Locate(block->size_, next);
    Node *node = next[0]->nexts_[0];
    bool found = false;
    // std::cout << "Remove block : " << block << ", size : " << block->size_ << ", node size : " << node->block_->size_
    //           << std::endl;
    while (node != nullptr && node->size() == block->size_) {
      if (node->block_ == block) {
        found = true;
        break;
      }
      node = node->nexts_[0];
      // if (node->block_ == nullptr) {
      //   std::cout << "unknown branch" << std::endl;
      // }
    }
    if (found) {
      RemoveNode(node, next);
    } else {
      std::cout << "remove block failed : " << block << ", node : " << node << ", node->size : " << node->size()
                << ", block size : " << block->size_ << std::endl;
    }

    return found;
  }

 public:
  // Locate position that less than key.
  void Locate(size_t size, Node *next[]) {
    Node *cur = head_;
    for (int i = LIST_LEVEL - 1; i >= 0; i--) {
      while (cur->nexts_[i] != nullptr && cur->nexts_[i]->size() < size) {
        cur = cur->nexts_[i];
      }
      next[i] = cur;
    }
  }

 private:
  Node *head_;

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
  ~SkipListAllocator() {}
  size_t MIN_SPLIT_SIZE = 512;

  void *Alloc(size_t size, uint32_t stream_id = 0) override;

  bool Free(void *addr) override;

  void Dump() {
    std::cout << "========dump======" << std::endl;
    free_blocks_[0].Dump();
    std::cout << "total_block_ size : " << total_block_.size() << std::endl;
    for (auto block : total_block_) {
      std::cout << "\taddr : " << block.first << ", size : " << block.second->size_
                << ", status : " << block.second->status_ << std::endl;
    }
  }

 private:
  std::unordered_map<uint32_t, SkipList> free_blocks_;
  std::unordered_map<void *, BlockRawPtr> total_block_;
  std::mutex mutex_;
};