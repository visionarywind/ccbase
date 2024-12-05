/**
 * Copyright 2024 Huawei Technologies Co., Ltd
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
#pragma once

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "dynamic_memory_pool.h"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define IntToSize(x) static_cast<size_t>(x)

#define MS_LIKELY likely
#define MS_UNLIKELY unlikely

constexpr size_t kDecimalPrecision = 3;

class AbstractDynamicMemPool;

struct JsonBuilder {
  JsonBuilder() { buffer_ << "{"; }

  template <typename T>
  void Append(std::string key, T value) {
    buffer_ << "\"" << key << "\":" << value << ",";
  }

  std::string ToString() {
    buffer_.seekp(-1, buffer_.cur);
    buffer_ << "}";
    return buffer_.str();
  }

  std::stringstream buffer_;
};

class Lock {
 public:
  inline void lock() {
    while (locked.test_and_set(std::memory_order_acquire)) {
    }
  }

  inline void unlock() { locked.clear(std::memory_order_release); }

 protected:
  std::atomic_flag locked = ATOMIC_FLAG_INIT;
};

class BACKEND_EXPORT LockGuard {
 public:
  explicit LockGuard(const Lock &lock);
  ~LockGuard();

 private:
  Lock *lock_;
};

struct MemBlock;

using MemBufStatus = DynamicMemBufStatus;
struct BACKEND_EXPORT MemBuf : EventBase {
  explicit MemBuf(size_t size, void *addr, uint32_t stream_id, MemBlock *mem_block, MemBufStatus status);

  MemBuf() = delete;
  MemBuf(const MemBuf &) = delete;
  MemBuf &operator=(const MemBuf &) = delete;

  ~MemBuf();

  inline void Link(MemBuf *prev, MemBuf *next) {
    if (prev != nullptr) {
      prev->next_ = this;
      this->prev_ = prev;
    }
    if (next != nullptr) {
      next->prev_ = this;
      this->next_ = next;
    }
  }

  inline void Unlink() {
    if (prev_ != nullptr) {
      prev_->next_ = next_;
    }
    if (next_ != nullptr) {
      next_->prev_ = prev_;
    }
    prev_ = nullptr;
    next_ = nullptr;
  }

  inline void SetDebugInfo() {
    owner_name_ = DynamicMemAllocatorDebugInfo::GetDebugInfo().name_;
    alloc_type_ = DynamicMemAllocatorDebugInfo::GetDebugInfo().type_;
  }

  std::string ToJson() {
    JsonBuilder builder;
    builder.Append("addr_", addr_);
    builder.Append("size_", size_);
    builder.Append("stream_id_", stream_id_);
    builder.Append("status_", DynamicMemBufStatusToString(status_));
    builder.Append("owner_name_", owner_name_);
    builder.Append("alloc_type_", AllocatorTypeToString(alloc_type_));
    return builder.ToString();
  }

  MemBuf *prev_;
  MemBuf *next_;

  size_t size_;
  void *addr_;
  uint32_t stream_id_;
  MemBlock *mem_block_;

  MemBufStatus status_;

  std::string owner_name_;
  AllocatorType alloc_type_{AllocatorType::kOther};
};

struct MemBufComparator {
  bool operator()(MemBuf *const &left, MemBuf *const &right) const {
    return (left->size_ != right->size_) ? left->size_ < right->size_ : left->addr_ < right->addr_;
  }
};

struct MemBlock {
  explicit MemBlock(size_t size, void *addr, uint32_t stream_id) : size_(size), addr_(addr), stream_id_(stream_id) {
    min_addr_ = nullptr;
    max_addr_ = nullptr;
  }

  MemBlock() = delete;
  MemBlock(const MemBlock &) = delete;
  MemBlock &operator=(const MemBlock &) = delete;

  ~MemBlock() = default;

  inline void UpdateBorderAddr(MemBuf *mem_buf) {
    if (min_addr_ == nullptr) {
      min_addr_ = mem_buf->addr_;
    } else {
      min_addr_ = std::min(min_addr_, mem_buf->addr_);
    }
    void *right_addr = static_cast<uint8_t *>(mem_buf->addr_) + mem_buf->size_;
    max_addr_ = std::max(max_addr_, right_addr);
  }

  inline size_t ActualPeakSize() {
    if (min_addr_ == nullptr || max_addr_ == nullptr) {
      return 0;
    }
    return static_cast<uint8_t *>(max_addr_) - static_cast<uint8_t *>(min_addr_);
  }

  std::string ToJson() {
    JsonBuilder builder;
    builder.Append("addr_", addr_);
    builder.Append("size_", size_);
    builder.Append("stream_id_", stream_id_);
    builder.Append("min_addr_", min_addr_);
    builder.Append("max_addr_", max_addr_);
    return builder.ToString();
  }

  size_t size_;
  void *addr_;
  uint32_t stream_id_;

  void *min_addr_;
  void *max_addr_;
};

struct MemStat {
  MemStat() { Reset(); }

  MemStat(const MemStat &) = delete;
  MemStat &operator=(const MemStat &) = delete;

  void Reset() {
    used_size_ = 0;
    peak_size_ = 0;
    alloc_size_ = 0;

    used_by_event_size_ = 0;
    eager_free_size_ = 0;

    temp_used_size_ = 0;
    temp_used_by_event_size_ = 0;
    temp_peak_size_ = 0;
    temp_alloc_size_ = 0;
  }

  inline size_t IdleSize() const { return alloc_size_ - used_size_; }

  inline void UpdatePeakSize() {
    peak_size_ = std::max(peak_size_, used_size_);
    if (used_size_ > temp_used_size_) {
      temp_peak_size_ = std::max(temp_peak_size_, used_size_ - temp_used_size_);
    }
  }

  std::string ToJson() const {
    JsonBuilder builder;
    builder.Append("used_size_", used_size_);
    builder.Append("peak_size_", peak_size_);
    builder.Append("alloc_size_", alloc_size_);
    builder.Append("idle_size_", IdleSize());
    builder.Append("used_by_event_size_", used_by_event_size_);
    builder.Append("eager_free_size_", eager_free_size_);
    return builder.ToString();
  }

  std::string ToReadableString() const {
    JsonBuilder builder;
    builder.Append("in used mem", Format(used_size_));
    builder.Append("peak used mem", Format(peak_size_));
    builder.Append("alloc mem", Format(alloc_size_));
    builder.Append("idle mem", Format(IdleSize()));
    builder.Append("used by event mem", Format(used_by_event_size_));
    builder.Append("eager free mem", Format(eager_free_size_));
    return builder.ToString();
  }

  std::string Format(size_t size) const {
    auto str = std::to_string(size * 1.0 / kMBToByte);
    return str.substr(0, str.find(".") + kDecimalPrecision) + "MB";
  }

  size_t used_size_;
  size_t peak_size_;
  size_t alloc_size_;

  size_t used_by_event_size_;
  size_t eager_free_size_;

  size_t temp_used_size_;
  size_t temp_used_by_event_size_;
  size_t temp_peak_size_;
  size_t temp_alloc_size_;
};

class MemBufAllocator {
 public:
  explicit MemBufAllocator(std::function<MemBlock *(size_t)> mem_block_expander,
                           std::function<bool(MemBlock *)> mem_block_cleaner,
                           std::function<size_t(size_t size, void *addr)> mem_mapper,
                           std::function<size_t(void *addr, size_t size)> mem_eager_freer, bool enable_eager_free,
                           bool is_persistent, uint32_t stream_id)
      : mem_block_expander_(mem_block_expander),
        mem_block_cleaner_(mem_block_cleaner),
        mem_mapper_(mem_mapper),
        mem_eager_freer_(mem_eager_freer),
        enable_eager_free_(enable_eager_free),
        stream_id_(stream_id),
        is_persistent_(is_persistent) {
    search_key_ = new MemBuf(0, nullptr, 0, nullptr, MemBufStatus::kMemBufIdle);
  }

  MemBufAllocator() = delete;
  MemBufAllocator(const MemBufAllocator &) = delete;
  MemBufAllocator &operator=(const MemBufAllocator &) = delete;

  ~MemBufAllocator();

  void ReleaseDeviceRes();

  MemBuf *Malloc(size_t size);
  bool Free(MemBuf *mem_buf, MemBufStatus target_status = MemBufStatus::kMemBufIdle);
  MemBuf *MallocExpandBlock(size_t size);
  const std::pair<size_t, size_t> FreeIdleMemsByEagerFree();

  std::string DumpStateInfo() const;
  std::string DumpDebugInfo() const;

  size_t ActualPeakSize() const {
    size_t peak_size = 0;
    for (auto mem_block : mem_blocks_) {
      peak_size += mem_block->ActualPeakSize();
    }
    return peak_size;
  }

  std::string BriefInfo() const {
    std::stringstream ss;
    ss << "Mem buf allocator, is persistent : " << is_persistent_ << ", stream id : " << stream_id_ << ".";
    return ss.str();
  }

  uint32_t stream_id() { return stream_id_; }

  bool is_persistent() { return is_persistent_; }
#ifndef ENABLE_TEST

 protected:
#endif
  inline MemBuf *MapAndSplitMemBuf(MemBuf *candidate, size_t size);
  inline MemBlock *ExpandBlock(size_t size);

  std::function<MemBlock *(size_t)> mem_block_expander_;
  std::function<bool(MemBlock *)> mem_block_cleaner_;
  std::function<size_t(size_t size, void *addr)> mem_mapper_;
  std::function<size_t(void *addr, size_t size)> mem_eager_freer_;
  bool enable_eager_free_;

  std::list<MemBlock *> mem_blocks_;
  std::set<MemBuf *, MemBufComparator> free_mem_bufs_;
  std::set<MemBuf *, MemBufComparator> eager_free_mem_bufs_;

 private:
  MemBuf *search_key_;

  uint32_t stream_id_;
  bool is_persistent_;

  friend AbstractDynamicMemPool;
};
using MemBufAllocatorPtr = std::shared_ptr<MemBufAllocator>;

class BACKEND_EXPORT AbstractDynamicMemPool : virtual public DynamicMemPool {
 public:
  explicit AbstractDynamicMemPool();
  ~AbstractDynamicMemPool() override = default;

  void ReleaseDeviceRes() override;

  // The main program entry of memory alloc.
  DeviceMemPtr AllocTensorMem(size_t size, bool from_persistent_mem = false, bool need_recycle = false,
                              uint32_t stream_id = kDefaultStreamIndex) override;

  // Alloc mem buf from mem pool, return mem buf and its allocator
  virtual std::pair<MemBuf *, MemBufAllocator *> AllocMemBuf(size_t align_size, bool from_persistent_mem = false,
                                                             uint32_t stream_id = kDefaultStreamIndex);

  // The main program entry of continuous memory alloc.
  std::vector<DeviceMemPtr> AllocContinuousTensorMem(const std::vector<size_t> &size_list,
                                                     uint32_t stream_id = kDefaultStreamIndex) override;
  // The main program entry of memory free.
  void FreeTensorMem(const DeviceMemPtr &device_addr) override;
  bool DoFreeTensorMem(const DeviceMemPtr &device_addr) override;
  // The main program entry of part memory free and part memory keep.
  void FreePartTensorMems(const std::vector<DeviceMemPtr> &free_addrs, const std::vector<DeviceMemPtr> &keep_addrs,
                          const std::vector<size_t> &keep_addr_sizes) override;
  virtual std::vector<MemBuf *> DoFreePartTensorMems(const std::vector<DeviceMemPtr> &free_addrs,
                                                     const std::vector<DeviceMemPtr> &keep_addrs,
                                                     const std::vector<size_t> &keep_addr_sizes) override;

  // Element in vector : memory_stream_id, address
  bool RecordEvent(int64_t task_id_on_stream, uint32_t user_stream_id,
                   const std::vector<std::pair<uint32_t, DeviceMemPtr>> &memory_stream_addresses,
                   const DeviceEventPtr &event) override;
  bool WaitEvent(int64_t task_id_on_stream, uint32_t user_stream_id, uint32_t memory_stream_id) override;
  bool WaitEvent(int64_t task_id_on_stream, uint32_t memory_stream_id) override;
  bool SyncAllEvents() override;
  bool DoSyncAllEvents();

  size_t CalMemBlockAllocSize(size_t size, bool from_persistent_mem, bool need_recycle = false) override;
  void SetMemAllocUintSize(size_t common_size, size_t persist_size = kDynamicMemAllocUnitSize) override {
    common_unit_size_ = common_size;
    persist_unit_size_ = persist_size;
  }
  size_t MemAllocUnitSize(bool from_persistent_mem = false) const override {
    return from_persistent_mem ? persist_unit_size_ : common_unit_size_;
  }

  void DefragMemory() override;

  void DumpDynamicMemPoolStateInfo() override;
  std::string DynamicMemPoolStateInfo() const;
  void DumpDynamicMemPoolDebugInfo() override;

  // The statistics information.
  size_t TotalMemStatistics() const override;
  size_t TotalUsedMemStatistics() const override;
  size_t TotalUsedByEventMemStatistics() const override;
  size_t TotalIdleMemStatistics() const override;
  size_t TotalEagerFreeMemStatistics() const override;
  size_t UsedMemPeakStatistics() const override;
  size_t MaxMemAllocatedStatistics() const override;
  size_t MaxMemReservedStatistics() const override;
  size_t ActualPeakStatistics() const override;
  std::unordered_map<std::string, std::size_t> BlockCountsStatistics() const override;
  std::unordered_map<std::string, std::size_t> BlockUnitSizeStatistics() const override;
  std::unordered_map<DeviceMemPtr, std::unordered_map<std::string, size_t>> CommonMemBlocksInfoStatistics()
    const override;
  std::unordered_map<DeviceMemPtr, std::unordered_map<std::string, size_t>> PersistentMemBlocksInfoStatistics()
    const override;
  void ResetMaxMemReserved() override;
  void ResetMaxMemAllocated() override;

  const bool IsEnableVmm() const override { return enable_vmm_; }

  void SetEnableVmm(bool enable_vmm) override { enable_vmm_ = enable_vmm; }

  // Get method for proxy.
  std::unordered_map<void *, std::pair<MemBuf *, MemBufAllocator *>> &addr_mem_buf_allocators() {
    return addr_mem_buf_allocators_;
  }

  std::unordered_map<std::pair<uint32_t, uint32_t>, std::set<MemBuf *>, pair_hash> &stream_pair_mem_bufs() {
    return stream_pair_mem_bufs_;
  }

  const std::pair<size_t, size_t> FreeIdleMemsByEagerFree() override;

  MemStat &mem_stat() { return mem_stat_; }

  Lock &lock() { return lock_; }

 protected:
  MemBufAllocatorPtr GenerateAllocator(bool is_persistent, uint32_t stream_id);
  inline MemBufAllocator *GetMemBufAllocator(size_t size, bool from_persistent_mem, uint32_t stream_id);
#ifndef ENABLE_TEST

 protected:
#else

 public:
#endif
  MemBufAllocatorPtr tiny_allocator_;

  std::map<std::pair<bool, uint32_t>, MemBufAllocatorPtr> stream_id_allocators_;
  std::unordered_map<void *, std::pair<MemBuf *, MemBufAllocator *>> addr_mem_buf_allocators_;
  std::unordered_map<std::pair<uint32_t, uint32_t>, std::set<MemBuf *>, pair_hash> stream_pair_mem_bufs_;
  MemStat mem_stat_;

  bool enable_vmm_{false};
  size_t common_unit_size_{kDynamicMemAllocUnitSize};
  size_t persist_unit_size_{kDynamicMemAllocUnitSize};

  size_t eager_free_count_{0};
  size_t last_eager_free_count_{0};
  Lock lock_;
};

struct BACKEND_EXPORT MemoryTimeEvent {
  // Creation time of address in ns.
  uint64_t created_at_{0};

  // Device address.
  void *addr_{nullptr};

  // Size of memory allocation.
  size_t size_{0};

  // Used size of memory pool.
  size_t used_size_{0};

  // Peak size of memory pool.
  size_t peak_size_{0};

  // Allocate size of memory pool.
  size_t alloc_size_{0};

  // Memory size that referred by event.
  size_t used_by_event_size_{0};

  // Eager free memory size.
  size_t eager_free_size_{0};

  // Whether allocation from persistent memory.
  uint8_t from_persistent_{false};

  // Whether allocated memory is persistent.
  uint8_t is_persistent_{false};

  // pynative or graph or ge.
  uint8_t run_mode_{0};

  // Data type of this address.
  uint8_t alloc_type_;

  // Stream id of address.
  uint32_t stream_id_{0};

  // Owner of this address.
  std::string owner_;

  std::string ToJson() {
    JsonBuilder builder;
    builder.Append("created_at_", created_at_);
    builder.Append("addr_", addr_);
    builder.Append("size_", size_);
    builder.Append("from_persistent_", from_persistent_);
    builder.Append("stream_id_", stream_id_);
    builder.Append("run_mode_", run_mode_);
    builder.Append("used_size_", used_size_);
    builder.Append("peak_size_", peak_size_);
    builder.Append("alloc_size_", alloc_size_);
    builder.Append("used_by_event_size_", used_by_event_size_);
    builder.Append("eager_free_size_", eager_free_size_);
    builder.Append("owner_", owner_);
    builder.Append("alloc_type_", alloc_type_);
    return builder.ToString();
  }
};
using MemoryTimeEventPtr = std::shared_ptr<MemoryTimeEvent>;

class BACKEND_EXPORT AbstractEnhancedDynamicMemPool : public AbstractDynamicMemPool {
 public:
  AbstractEnhancedDynamicMemPool();
  AbstractEnhancedDynamicMemPool(const AbstractEnhancedDynamicMemPool &) = delete;
  AbstractEnhancedDynamicMemPool &operator=(const AbstractEnhancedDynamicMemPool &) = delete;
  ~AbstractEnhancedDynamicMemPool() override = default;

  // Report memory pool stat info for enhanced processing.
  virtual void ReportMemoryPoolInfo();

  bool IsEnableTimeEvent() override { return enable_time_event_; }

  void SetEnableTimeEvent(bool enable_time_event) override { enable_time_event_ = enable_time_event; }

  virtual MemoryTimeEventPtr GenAllocateMemoryTimeEvent(const void *addr, size_t size, uint32_t stream_id,
                                                        bool from_persistent, bool is_persistent);

  virtual MemoryTimeEventPtr GenFreeMemoryTimeEvent(const void *addr);

 private:
  std::atomic<bool> enable_time_event_{false};
};
