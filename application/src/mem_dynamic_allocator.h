/**
 * Copyright 2019-2023 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_CCSRC_BACKEND_OPTIMIZER_MEM_REUSE_MEM_DYNAMIC_ALLOCATOR_H_
#define MINDSPORE_CCSRC_BACKEND_OPTIMIZER_MEM_REUSE_MEM_DYNAMIC_ALLOCATOR_H_

#include <memory>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <utility>
#include <thread>
#include <mutex>
#include <string>
#include <tuple>
// #include "utils/ms_utils.h"
// #include "include/backend/visible.h"
// #include "include/common/utils/stream_util.h"
// #ifdef __APPLE__
// #include "mindrt/include/async/spinlock.h"
// #endif

inline uint8_t *AddressOffset(void *address, size_t offset) { return static_cast<uint8_t *>(address) + offset; }

inline size_t CalAddressOffset(void *dst_address, void *ori_address) {
  return static_cast<uint8_t *>(dst_address) - static_cast<uint8_t *>(ori_address);
}

constexpr uint32_t kDefaultStreamIndex = 0;
constexpr size_t kDefaultMempoolBlockSize = 1 << 31;

// namespace mindspore {
// namespace device {
// The status of memory buf.
enum class DynamicMemBufStatus : int { kMemBufIdle, kMemBufUsed, kMemBufEagerFree };
// Memory allocator type is used to record the memory classification statistics information.
enum class AllocatorType : int { kWeight, kConstantValue, kKernelOutput, kGraphOutput, kOther };
constexpr int kAllocatorTypeNum = 5;
// Alloc memory aligned according to 512 bytes.
constexpr size_t kDynamicMemAlignSize = 512;
// The minimum unit size (1G) of memory block used for dynamic extend.
constexpr size_t kDynamicMemAllocUnitSize = 1024 << 20;

// The Comparator of device address from small to large.
using DeviceMemPtr = void(*);
struct DeviceAddrCmp {
  bool operator()(const DeviceMemPtr &addr1, const DeviceMemPtr &addr2) const { return addr1 < addr2; }
};

// The AllocatorDebugInfo wrapper which is the local thread for the dynamic memory pool.
class DynamicMemAllocatorDebugInfo;
// Memory buf is the smallest operation object of dynamic memory pool.
struct DynamicMemBuf;
using DynamicMemBufPtr = std::shared_ptr<DynamicMemBuf>;
// Multimap key is the tensor size, for finding the idle memory buf by tensor size.
using SizeMapMemBuf = std::multimap<size_t, DynamicMemBufPtr>;
// Map key is the device address, for finding the used memory buf in memory block by device address.
using DeviceAddrMapMemBuf = std::map<DeviceMemPtr, DynamicMemBufPtr, DeviceAddrCmp>;
// Memory block is composed of memory buf.
class DynamicMemBlock;
using DynamicMemBlockPtr = std::shared_ptr<DynamicMemBlock>;

struct MemStatusManager;
using MemStatusManagerPtr = std::shared_ptr<MemStatusManager>;

// The main class of dynamic memory pool.
class DynamicMemPoolBestFit {
 public:
  DynamicMemPoolBestFit()
      : persistent_mem_(std::make_shared<MemStatusManager>()), common_mem_(std::make_shared<MemStatusManager>()) {}
  virtual ~DynamicMemPoolBestFit();

  // The main program entry of memory alloc.
  DeviceMemPtr AllocTensorMem(size_t size, bool from_persistent_mem = false, bool need_recycle = false,
                              uint32_t stream_id = kDefaultStreamIndex);
  // The main program entry of continuous memory alloc.
  std::vector<DeviceMemPtr> AllocContinuousTensorMem(const std::vector<size_t> &size_list,
                                                     uint32_t stream_id = kDefaultStreamIndex);
  // The main program entry of memory free.
  void FreeTensorMem(const DeviceMemPtr &device_addr);
  // The main program entry of part memorys free and part memorys keep.
  void FreePartTensorMems(const std::vector<DeviceMemPtr> &free_addrs, const std::vector<DeviceMemPtr> &keep_addrs,
                          const std::vector<size_t> &keep_addr_sizes);

  // Release the real device memory.
  void ReleaseDeviceRes();

  // Get the minimum memory unit size using for dynamic extend.
  size_t MemAllocUnitSize(bool from_persistent_mem = false) const;
  // Set the minimum memory unit size using for dynamic extend.
  void SetMemAllocUintSize(size_t common_size, size_t persist_size = kDynamicMemAllocUnitSize);

  // The statistics information.
  size_t TotalMemStatistics() const;
  size_t TotalUsedMemStatistics() const;
  size_t TotalIdleMemStatistics() const;
  size_t TotalEagerFreeMemStatistics() const;
  size_t UsedMemPeakStatistics() const;

  // Display the brief state information of memory block and memory buf.
  void DumpDynamicMemPoolStateInfo();
  // Display the detailed debug information of memory block and memory buf.
  void DumpDynamicMemPoolDebugInfo();

  // The related interface of device memory real operation, needs override by device type.
  virtual size_t AllocDeviceMem(size_t size, DeviceMemPtr *addr) {
    *addr = malloc(size);
    return size;
  }
  virtual bool FreeDeviceMem(const DeviceMemPtr &addr) {
    free(addr);
    return true;
  }
  virtual size_t free_mem_size() { return SIZE_MAX; };
  virtual uint64_t total_mem_size() const { return SIZE_MAX; }
  // Set mem pool block size
  virtual void SetMemPoolBlockSize(size_t available_device_mem_size);
  virtual size_t GetMaxUsedMemSize() const { return 0; }
  // #ifdef WITH_BACKEND

 protected:
  // #endif
  const MemStatusManagerPtr &common_mem() const { return common_mem_; }
  const MemStatusManagerPtr &persistent_mem() const { return persistent_mem_; }
  void *GetMinUsedMemoryAddr() const;
  // The real size by memory alloc aligned.
  virtual size_t AlignMemorySize(size_t size) const;
  // Calculate memory block required alloc size when adding the memory block.
  virtual size_t CalMemBlockAllocSize(size_t size, bool from_persistent_mem, bool need_recycle = false);
  std::set<DeviceMemPtr> mem_bufs_;

  // The related interface of device memory eager free.
  virtual const bool IsEnableEagerFree() const { return false; }
  virtual const bool SyncAllStreams() { return false; }
  virtual size_t AllocDeviceMemByEagerFree(size_t size, DeviceMemPtr *addr) { return 0; }
  virtual size_t FreeDeviceMemByEagerFree(const DeviceMemPtr addr, const size_t size) { return 0; }
  const size_t FreeIdleMemsByEagerFree();

 private:
  // Find available memory buf from total pools by status, which contains idle and eager free.
  DeviceMemPtr FindAvailableMemBuf(size_t size, bool from_persistent_mem, uint32_t stream_id);
  // Find the target status memory buf from total pools by aligned size when memory alloc.
  DeviceMemPtr FindMemBufByStatus(size_t size, bool from_persistent_mem, DynamicMemBufStatus target_status,
                                  uint32_t stream_id);
  // Find the target status memory buf from specific pool by aligned size when memory alloc.
  DeviceMemPtr FindMemBufInSpecifiedMng(size_t size, bool from_persistent_mem, DynamicMemBufStatus target_status,
                                        uint32_t stream_id);

  // Add memory block and memory.
  DeviceMemPtr AddMemBlockAndMemBuf(size_t size, bool from_persistent_mem, bool need_recycle, uint32_t stream_id);
  // Add memory block and memory buf with eager free api.
  DeviceMemPtr AddMemBlockAndMemBufByEagerFree(size_t size, bool from_persistent_mem, uint32_t stream_id);
  // Add the memory block and memory buf when memory alloc not find the available memory buf.
  DeviceMemPtr CreateMemBlockAndMemBuf(size_t size, bool from_persistent_mem, DeviceMemPtr source_addr,
                                       size_t source_size, DynamicMemBufStatus mem_buf_status, uint32_t stream_id);

  // Judge whether need split the memory buf by alloc size and memory buf size.
  bool IsSplit(size_t tensor_size, size_t mem_buf_size) const;
  // Split the memory buf by alloc size.
  void SplitMemBuf(size_t size, const DynamicMemBufPtr &mem_buf, const MemStatusManagerPtr &mem_mng,
                   uint32_t stream_id);

  // Find the memory block by device address.
  DynamicMemBlockPtr FindMemBlock(const DeviceMemPtr &device_addr, const MemStatusManagerPtr &mem_mng) const;
  // The Comparator of memory block by device address, because memory blocks are arranged in order by device address.
  static bool CmpMemBlock(const DeviceMemPtr &device_addr, const DynamicMemBlockPtr &mem_block);

  // Free memory inner with no lock, the caller need lock.
  void FreeTensorMemInner(const DeviceMemPtr &device_addr);
  // Combine the memory buf when memory free, to avoid the memory fragmentation.
  void CombineMemBuf(const DeviceMemPtr &device_addr, DynamicMemBufStatus origin_status,
                     DynamicMemBufStatus target_status);
  // Fetch the mem info by the strict addr.
  std::tuple<DynamicMemBlockPtr, DeviceAddrMapMemBuf::iterator, MemStatusManagerPtr> FindByStrictAddr(
    const DeviceMemPtr &device_addr) const;
  // Erase memory buf by size and device address when memory buf is combined.
  void EraseMemBufByStatus(size_t size, const DeviceMemPtr &device_addr, const MemStatusManagerPtr &mem_mng,
                           DynamicMemBufStatus target_status, uint32_t stream_id) const;

  // Keep the part memorys by addr.
  void KeepTensorMemByAddr(const DeviceMemPtr &device_addr, size_t size);
  std::tuple<DynamicMemBlockPtr, DynamicMemBufPtr, MemStatusManagerPtr> FindByKeepAddr(
    const DeviceMemPtr &device_addr) const;
  DynamicMemBufPtr FindMemBufByKeepAddr(const DeviceMemPtr &device_addr, const DynamicMemBlockPtr &mem_block) const;

  // #ifdef __APPLE__
  //   // There are some problems with using mutex on Mac, use spinlocks instead.
  //   SpinLock spin_lock_;
  // #else
  // Support multi-thread.
  std::mutex mutex_;
  // #endif
  MemStatusManagerPtr persistent_mem_{nullptr};
  MemStatusManagerPtr common_mem_{nullptr};
  // In the graph mode, the unit size set in the context will be modified through the FetchMemUnitSize function, so it
  // needs to be changed back after that
  size_t config_unit_size_{kDynamicMemAllocUnitSize};
  // Flag for eager free routine. This flag set to false when initializing, and set to true when triggering oom.
  bool is_trigger_eager_free_{false};
};

struct DynamicMemBuf {
  DynamicMemBuf(DeviceMemPtr addr, DynamicMemBufStatus status, size_t size)
      : device_addr_(addr), status_(status), size_(size) {}
  DynamicMemBuf(DeviceMemPtr addr, DynamicMemBufStatus status, size_t size, const std::string &allocator_name,
                AllocatorType allocator_type)
      : device_addr_(addr),
        status_(status),
        size_(size),
        allocator_name_(allocator_name),
        allocator_type_{allocator_type} {}
  DeviceMemPtr device_addr_;
  DynamicMemBufStatus status_;
  size_t size_;

  // Debug info.
  std::string allocator_name_;
  AllocatorType allocator_type_{AllocatorType::kOther};
};

class DynamicMemBlock {
 public:
  DynamicMemBlock() = delete;
  DynamicMemBlock(DeviceMemPtr addr_base, size_t size, const uint32_t stream_id)
      : device_addr_base_(addr_base), mem_block_size_(size), stream_id_(stream_id) {}
  ~DynamicMemBlock() { block_all_mem_buf_map_.clear(); }
  const DeviceMemPtr &device_addr() const { return device_addr_base_; }
  size_t size() const { return mem_block_size_; }
#ifdef WITH_BACKEND

 private:
#endif
  friend class DynamicMemPoolBestFit;

  // The map of all memory buf in this memory block by device address.
  DeviceAddrMapMemBuf block_all_mem_buf_map_;

  DeviceMemPtr device_addr_base_{nullptr};

  size_t mem_block_size_{0};
  const uint32_t stream_id_;
};

struct DeviceState {
  // Memory allocated from device
  size_t total_mem_size_{0};
  // Memory in use
  size_t total_used_mem_size_{0};
  // Memory in idle.
  size_t total_idle_mem_size_{0};
  // Memory in eager free.
  size_t total_eager_free_mem_size_{0};
  // Maximum peak memory usage
  size_t used_mem_peak_size_{0};
};

struct MemStatusManager {
  bool Empty() const { return mem_block_list_.empty(); }

  void AddMemBlock(const DynamicMemBlockPtr &mem_block, uint32_t stream_id);
  void DoAddMemBlock(const DynamicMemBlockPtr &mem_block, std::vector<DynamicMemBlockPtr> *mem_block_list);

  SizeMapMemBuf &GetIdleMemBufMap(uint32_t stream_id) { return GetOrCreateSizeMapMemBuf(&idle_mem_bufs_, stream_id); }

  void AddIdleMemBuf(const DynamicMemBufPtr &mem_buf, uint32_t stream_id) {
    AddMemBuf(&idle_mem_bufs_, mem_buf, stream_id);
  }
  bool RemoveIdleDeviceMem(const size_t size, const DeviceMemPtr &device_addr, uint32_t stream_id) {
    return RemoveDeviceMem(&idle_mem_bufs_, size, device_addr, stream_id);
  }
  SizeMapMemBuf &GetEagerFreeMemBuf(uint32_t stream_id) {
    return GetOrCreateSizeMapMemBuf(&eager_free_mem_bufs_, stream_id);
  }

  void AddEagerFreeMemBuf(const DynamicMemBufPtr &mem_buf, uint32_t stream_id) {
    AddMemBuf(&eager_free_mem_bufs_, mem_buf, stream_id);
  }
  bool RemoveEagerFreeDeviceMem(const size_t size, const DeviceMemPtr &device_addr, uint32_t stream_id) {
    return RemoveDeviceMem(&eager_free_mem_bufs_, size, device_addr, stream_id);
  }
  SizeMapMemBuf &GetEagerFreeMemBufMap(uint32_t stream_id) {
    return GetOrCreateSizeMapMemBuf(&eager_free_mem_bufs_, stream_id);
  }

  void AddMemBuf(std::map<uint32_t, SizeMapMemBuf> *container, const DynamicMemBufPtr &mem_buf, uint32_t stream_id);
  bool RemoveDeviceMem(std::map<uint32_t, SizeMapMemBuf> *container, const size_t size, const DeviceMemPtr &device_addr,
                       uint32_t stream_id);
  SizeMapMemBuf &GetOrCreateSizeMapMemBuf(std::map<uint32_t, SizeMapMemBuf> *container, uint32_t stream_id);
  void Clear() noexcept;

  size_t unit_size_{kDynamicMemAllocUnitSize};
  // Mem pool state
  DeviceState mps_;

  std::vector<DynamicMemBlockPtr> mem_block_list_;
  std::map<uint32_t, std::vector<DynamicMemBlockPtr>> mem_blocks_;

  std::map<uint32_t, SizeMapMemBuf> idle_mem_bufs_;
  std::map<uint32_t, SizeMapMemBuf> eager_free_mem_bufs_;
};
//}  // namespace device
//}  // namespace mindspore
#endif  // MINDSPORE_CCSRC_BACKEND_OPTIMIZER_MEM_REUSE_MEM_DYNAMIC_ALLOCATOR_H_
