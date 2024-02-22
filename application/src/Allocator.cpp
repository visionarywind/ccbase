#include "Allocator.h"

inline size_t align(size_t size) {
  if (size == 0) {
    return 512;
  }
  return ((size + 512 - 1) / 512) * 512;
}

void *DefaultAllocator::Alloc(size_t size, uint32_t stream_id) {
  size = align(size);
  std::lock_guard<std::mutex> locker(mutex_);
  auto &free_blocks = free_blocks_[stream_id];
  auto key = new Block(nullptr, size, stream_id);
  auto it = free_blocks.lower_bound(key);
  delete key;
  Block *block = nullptr;
  if (it == free_blocks.end() || (*it)->stream_id_ != stream_id) {
    void *addr = MemAlloc(1 << 31);
    block = new Block(addr, 1 << 31, 0);
    total_block_.emplace(addr, block);
  } else {
    block = *it;
    free_blocks.erase(it);
  }

  size_t remaining = block->size_ - size;
  if (remaining >= MIN_SPLIT_SIZE) {
    auto remaining_addr = static_cast<int8_t *>(block->addr_) + size;
    auto remaining_block = new Block(remaining_addr, remaining, 1);
    block->next_ = remaining_block;
    remaining_block->prev_ = block;

    free_blocks.emplace(remaining_block);
    total_block_.emplace(remaining_block->addr_, remaining_block);
    block->size_ = size;
  }

  free_blocks.emplace(block);
  return block->addr_;
}

bool DefaultAllocator::Free(void *addr) {
  std::lock_guard<std::mutex> locker(mutex_);
  auto it = total_block_.find(addr);
  if (it != total_block_.end()) {
    auto block = it->second;
    block->status_ = 0;

    auto &free_blocks = free_blocks_[block->stream_id_];

    auto prev_block = block->prev_;
    if (prev_block != nullptr) {
      if (prev_block->status_ == 0) {
        free_blocks.erase(prev_block);
        total_block_.erase(prev_block->addr_);
        block->addr_ = prev_block->addr_;
        block->size_ += prev_block->size_;
        delete prev_block;
      }
    }
    auto next_block = block->next_;
    if (next_block != nullptr) {
      if (next_block->status_ == 0) {
        free_blocks.erase(next_block);
        total_block_.erase(next_block->addr_);
        block->size_ += next_block->size_;
        delete next_block;
      }
    }

    free_blocks.insert(block);
    return true;
  }

  return false;
}