#pragma once

#include "abstract_dynamic_memory_pool.h"

#include <iostream>
#include <map>
#include <set>

struct Pool {
  std::set<MemBuf *, MemBufComparator> free_mem_bufs_;
};

struct PtaMemoryPool {
  // The main program entry of memory alloc.
  void AllocTensorMem(size_t size, bool from_persistent_mem = false, bool need_recycle = false,
                      uint32_t stream_id = 0) {}

  // The main program entry of memory free.
  void FreeTensorMem(const void *device_addr) {}
};