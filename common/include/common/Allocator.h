#pragma once

#include <atomic>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

struct Segment {
  void *addr;
  size_t size;
};

struct Block {
  void *addr;
  size_t size;
  Block *prev;
  Block *next;
  int status;
};

class Allocator {
 public:
  virtual void *Malloc(const std::size_t size) = 0;
  virtual void Free(void *ptr) = 0;
};

constexpr std::size_t kAlignSize512Kb = 1 << 19;
constexpr std::size_t kAlignSize1Mb = 1 << 20;
constexpr std::size_t kAlignSize2Mb = 1 << 21;

class AlignedAllocator : public Allocator {
 public:
  Allocator(const std::size_t align_size = kDefaultAlignSize) : align_size_(align_size) {}

  const int Align(const std::size_t size) {
    if (size <= align_size_) {
      return align_size_;
    }
    return ((size + align_size_ - 1) / align_size_) * align_size_;
  }

 protected:
  const std::size_t align_size_;
};

class AbstractAllocator : public AlignedAllocator {
 protected:
  atomic<std::size_t> used_size_;
};

typedef bool (*Comparator)(const Block *, const Block *);
static bool BlockAddrComparator(const Block *left, const Block right) {}

class DirectAllocator : public AbstractAllocator {
 public:
  void *Malloc(const std::size_t size) override {
    auto align_size = Align(size);
    auto ret = mmap(nullptr, align_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (ret == MAP_FAILED) {
      return nullptr;
    }
    (void)pool_map_.emplace(ret, align_size);
    return ret;
  }
  void Free(void *ptr) override {
    if (pool_map_.count(ptr) == 0) {
      return;
    }
    auto ret = munmap(ptr, size_t);
    if (ret != MAP_FAILED) {
      pool_map_.erase(ptr);
    }
  }

 private:
  std::map<void * addr, std::size_t> pool_map_;
};