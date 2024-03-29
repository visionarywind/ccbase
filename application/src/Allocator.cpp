#include "Allocator.h"

inline size_t align(size_t size) { return size != 0 ? (size + 512 - 1) & ~(512 - 1) : 512; }

auto key = new Block(nullptr, 0, 0);

void *DefaultAllocator::Alloc(size_t size, uint32_t stream_id) {
  // std::cout << "alloc size : " << size << std::endl;
  size = align(size);
  std::lock_guard<std::mutex> locker(mutex_);
  auto &free_blocks = free_blocks_[stream_id];
  // std::cout << "free_blocks size : " << free_blocks.size() << std::endl;
  key->size_ = size;
  key->stream_id_ = stream_id;
  auto it = free_blocks.lower_bound(key);
  Block *block = nullptr;
  if (it == free_blocks.end()) {
    void *addr = MemAlloc(1 << 30);
    // std::cout << "malloc addr : " << addr << std::endl;
    block = new Block(addr, 1 << 30, 0);
    total_block_.emplace(addr, block);
  } else {
    block = *it;
    // std::cout << "free_blocks erase : " << block->addr_ << std::endl;
    free_blocks.erase(it);
  }

  size_t remaining = block->size_ - size;
  if (remaining >= MIN_SPLIT_SIZE) {
    auto remaining_addr = static_cast<int8_t *>(block->addr_) + size;
    auto remaining_block = new Block(remaining_addr, remaining, 0);
    auto next = block->next_;
    if (next != nullptr) {
      next->prev_ = remaining_block;
      remaining_block->next_ = next;
    }
    block->next_ = remaining_block;
    remaining_block->prev_ = block;

    free_blocks.emplace(remaining_block);
    // remaining_block->Print();
    // std::cout << "free_blocks insert : " << remaining_block->addr_ << std::endl;
    total_block_.emplace(remaining_block->addr_, remaining_block);
    block->size_ = size;
  }
  block->status_ = 1;

  // std::cout << "alloc free_blocks size : " << free_blocks.size() << std::endl;
  // std::cout << "alloc block : " << block << ", addr : " << block->addr_ << ", prev " << block->prev_ << ", next "
  //           << block->next_ << std::endl;
  return block->addr_;
}

bool DefaultAllocator::Free(void *addr) {
  // std::cout << "free addr : " << addr << std::endl;
  std::lock_guard<std::mutex> locker(mutex_);
  auto it = total_block_.find(addr);
  if (it != total_block_.end()) {
    auto block = it->second;
    block->status_ = 0;
    // std::cout << "locate block : " << block << ", prev : " << block->prev_ << ", next : " << block->next_
    //           << ", stream_id : " << block->stream_id_ << std::endl;

    auto &free_blocks = free_blocks_[block->stream_id_];
    // std::cout << "free blocks size : " << free_blocks.size() << std::endl;

    auto prev_block = block->prev_;
    if (prev_block != nullptr) {
      // prev_block->Print();
      if (prev_block->status_ == 0) {
        // erase prev block pointer
        auto prev = prev_block->prev_;
        block->prev_ = prev;
        if (prev != nullptr) {
          prev->next_ = block;
        }

        free_blocks.erase(prev_block);
        total_block_.erase(prev_block->addr_);
        block->addr_ = prev_block->addr_;
        block->size_ += prev_block->size_;
        // prev_block->Print();
        // std::cout << "remove prev : " << prev_block << std::endl;
        delete prev_block;
      } else {
        // std::cout << "prev_block->status_ : " << prev_block->status_ << " is not ok" << std::endl;
      }
    }
    auto next_block = block->next_;
    if (next_block != nullptr) {
      // next_block->Print();
      if (next_block->status_ == 0) {
        // erase next block pointer
        auto next = next_block->next_;
        block->next_ = next;
        if (next != nullptr) {
          next->prev_ = block;
        }

        block->size_ += next_block->size_;
        free_blocks.erase(next_block);
        total_block_.erase(next_block->addr_);
        // next_block->Print();
        // std::cout << "remove next : " << next_block << std::endl;
        delete next_block;
      } else {
        // std::cout << "next_block : " << next_block << ", size : " << next_block->size_
        //           << ", status_ : " << next_block->status_ << " is not ok" << std::endl;
      }
    }

    // std::cout << "insert back : " << free_blocks.size() << std::endl;
    // block->Print();
    free_blocks.insert(block);
    // std::cout << "free_blocks size : " << free_blocks.size() << std::endl;
    return true;
  } else {
    std::cout << "unknown addr : " << addr << std::endl;
  }
  // std::cout << "free_blocks failed" << std::endl;

  return false;
}