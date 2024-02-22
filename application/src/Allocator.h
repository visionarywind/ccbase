#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <set>

struct Block {
  Block(void *addr, size_t size, int status) : addr_(addr), size_(size), status_(status) {}
  void *addr_;
  size_t size_;
  int status_;  // 0 : free, 1 : used, 2 : eager free.

  int stream_id_;
  struct Block *prev_{nullptr};
  struct Block *next_{nullptr};
};
using BlockRawPtr = struct Block *;

class BlockComparator {
 public:
  bool operator()(const BlockRawPtr &left, const BlockRawPtr &right) const {
    // stream id is not used currently
    // if (left->stream_id_ != right->stream_id_) {
    //   return left->stream_id_ < right->stream_id_;
    // }
    if (left->size_ == right->size_) {
      return left->addr_ < right->addr_;
    }
    return left->size_ < right->size_;
  }

  bool operator()(const size_t size, const BlockRawPtr &block) const { return size < block->size_; }
  bool operator()(const BlockRawPtr &block, const size_t size) const { return block->size_ < size; }
};

struct MemBlock {
  MemBlock(void *addr, size_t size) : addr_(addr), size_(size) {}
  void *addr_;
  size_t size_;
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