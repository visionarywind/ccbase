#pragma once

#include "abstract_dynamic_memory_pool.h"
#include "ascend_vmm_adapter.h"

#include <iostream>

class MemoryPool : public AbstractDynamicMemPool {
 public:
  explicit MemoryPool() { SetEnableVmm(AscendVmmAdapter::GetInstance().IsEnabled()); }
  ~MemoryPool() override = default;

  size_t AllocDeviceMem(size_t size, DeviceMemPtr *addr) override {
    *addr = malloc(size);
    alloc_infos_.emplace(addr, size);
    std::cout << "alloc device mem, size : " << size << ", addr : " << *addr << std::endl;
    return size;
  }

  bool FreeDeviceMem(const DeviceMemPtr &addr) override {
    std::cout << "free device mem, addr : " << addr << std::endl;
    auto &&iter = alloc_infos_.find(addr);
    if (iter == alloc_infos_.end()) {
      return false;
    }
    alloc_infos_.erase(iter);
    return true;
  }

  size_t GetMaxUsedMemSize() const override { return kMaxUsedMemSize; }

  size_t GetVmmUsedMemSize() const override { return AscendVmmAdapter::GetInstance().GetAllocatedSize(); }

  size_t free_mem_size() override { return SIZE_MAX; }

  uint64_t total_mem_size() const override {
    // when enable eager free, this size effect block size, use small size.
    return total_mem_size_;
  }

  size_t ReservedMemorySize() {
    size_t reserved_memory_size = 0;
    for (auto &alloc_info : alloc_infos_) {
      reserved_memory_size += alloc_info.second;
    }
    return reserved_memory_size;
  }

 protected:
  // The related interface of device memory eager free.
  const bool IsEnableEagerFree() const override { return false; }

  const bool SyncAllStreams() override { return true; }

  size_t AllocDeviceMemByEagerFree(size_t size, DeviceMemPtr *addr) override {
    return AscendVmmAdapter::GetInstance().AllocDeviceMem(size, addr);
  }

  size_t FreeDeviceMemByEagerFree(const DeviceMemPtr addr, const size_t size) override {
    return AscendVmmAdapter::GetInstance().EagerFreeDeviceMem(addr, size);
  }

  size_t MmapDeviceMem(const size_t size, const DeviceMemPtr addr) override {
    return AscendVmmAdapter::GetInstance().MmapDeviceMem(size, addr, GetMaxUsedMemSize());
  }

 private:
  size_t vmm_mmap_size_{0};
  std::map<void *, size_t> alloc_infos_;

  size_t total_mem_size_{kMaxUsedMemSize};
};