#pragma once

struct Block {
  Block(void *addr, size_t size, int status) : addr_(addr), size_(size), status_(status) {}
  void *addr_;
  size_t size_;
  int status_;  // 0 : free, 1 : used, 2 : eager free.

  int stream_id_;
  struct Block *prev_{nullptr};
  struct Block *next_{nullptr};
};

struct MemBlock {
  MemBlock(void *addr, size_t size) : addr_(addr), size_(size) {}
  void *addr_;
  size_t size_;
};

public
class Allocator {
 public:
  virtual void *Alloc(size_t size) = 0;
  virtual bool Free(void *addr) = 0;

  virtual void *MemAlloc(size_t size) = 0;
  virtual bool MemFree(void *addr) = 0;
};

public
class MallocAllocator : public Allocator {
  void *MemAlloc(size_t size) override {
    void *addr = malloc(size);
    auto mem_block = new MemBlock(addr, size);
    (void)allocated_blocks_.emplace_back(mem_block);
    return addr;
  }

  bool MemFree(void *addr) override {
    auto it = std::find(allocated_blocks_.begin(), allocated_blocks_.end(),
                        [addr](struct MemBlock *mem_block) { return addr == mem_block->addr_; });
    if (it != allocated_blocks_.end()) {
      auto mem_block = it->second;
      allocated_blocks_.erase(mem_block);
      free(addr);
      delete mem_block;
      return true;
    }
    return false;
  }

 protected:
  std::vector<MemBlock> allocated_blocks_;
};

public
class DefaultAllocator : public MallocAllocator {
 public:
  constexpr size_t MIN_SPLIT_SIZE = 512;

  void *Alloc(size_t size) override {
    auto it = free_blocks_.lower_bound(size);
    if (it != free_blocks_.end()) {
      auto block = it->second;
      size_t extra_size = block->size_ - size;
      if (extra_size >= MIN_SPLIT_SIZE) {
            }
      return block->addr_;
    }
    return nullptr;
  }

  bool Free(void *addr) override {
    auto it = total_block_.find(addr);
    if (it != total_block_.end()) {
      auto block = it->secend;
      block->status = 1;

      auto prev_block = block->prev_;
      if (prev_block != nullptr) {
        if (prev_block->status == 0) {
          free_blocks_.erase(prev_block);
          total_block_.erase(prev_block->addr_);
          block->addr_ = prev_block->addr_;
          block->size_ += prev_block->size_;
          delete prev_block;
        }
      }
      auto next_block = block->next_;
      if (next_block != nullptr) {
        if (next_block->status == 0) {
          free_blocks_.erase(next_block);
          total_block_.erase(next_block->addr_);
          block->size_ += next_block->size_;
          delete next_block;
        }
      }

      free_blocks_.insert(block);
      return true;
    }

    return false;
  }

 private:
  std::set<Block *> free_blocks_;  // 需要排序函数
  std::map<void * addr, Block *> total_block_;

  Block head_;
};