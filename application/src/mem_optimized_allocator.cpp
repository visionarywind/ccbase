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

#include "mem_optimized_allocator.h"

#include <string>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <ostream>
#include <utility>

inline int64_t Get() {
  auto now_time = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::nanoseconds>(now_time.time_since_epoch()).count();
}

static const char kPersistentParamMem[] = "Persistent mem";
static const char kCommonMem[] = "Common mem";
constexpr size_t kGBToByte = 1024 << 20;
// The smallest memory request size, if it is smaller than this size, the device memory request may fail
// Set experience value to 10M
const size_t kMinimumAllocMem = 10 << 20;

thread_local AllocatorDebugInfo DynamicMemAllocatorDebugInfo::debug_info_;

static const std::map<DynamicMemBufStatus, std::string> kBufStatusString = {
  {DynamicMemBufStatus::kMemBufIdle, "idle"},
  {DynamicMemBufStatus::kMemBufUsed, "used"},
  {DynamicMemBufStatus::kMemBufEagerFree, "eager_free"}};

static const std::map<AllocatorType, std::string> kAllocatorTypeString = {
  {AllocatorType::kWeight, "weight"},
  {AllocatorType::kConstantValue, "constant value"},
  {AllocatorType::kKernelOutput, "kernel output"},
  {AllocatorType::kGraphOutput, "graph output"},
  {AllocatorType::kOther, "other"},
};

DynamicMemPoolBestFitOpt::~DynamicMemPoolBestFitOpt() {
  persistent_mem_->Clear();
  common_mem_->Clear();
}

DeviceMemPtr DynamicMemPoolBestFitOpt::AllocTensorMem(size_t size, bool from_persistent_mem, bool need_recycle,
                                                      uint32_t stream_id) {
  size_t align_size = AlignMemorySize(size);
  std::lock_guard<SpinLock> lock(spin_lock_);
  // Find the memory buf by tensor size, if not find, then add new memory block and memory buf.
  DeviceMemPtr device_addr = FindAvailableMemBuf(align_size, from_persistent_mem, stream_id);
  static bool init_recycle_memory = false;
  if (need_recycle && !init_recycle_memory) {
    // Force persist memory to be reserved when recycle memory is allocated for the first time
    init_recycle_memory = true;
    // MS_LOG(INFO) << "Init Recycle Memory";
    device_addr = nullptr;
  }
  if (device_addr == nullptr) {
    device_addr = AddMemBlockAndMemBuf(align_size, from_persistent_mem, need_recycle, stream_id);
  }
  // Alloc memory failed and dump the info.
  if (!device_addr) {
    DumpDynamicMemPoolStateInfo();
  }

  //   if (common::IsNeedProfileMemory()) {
  //     MS_LOG(WARNING) << "Need Profile Memory, Memory pool alloc, total mem: " << TotalMemStatistics()
  //                     << ", peak mem: " << UsedMemPeakStatistics() << ", in use mem: " << TotalUsedMemStatistics()
  //                     << ", device address addr: " << device_addr << ", size: " << size
  //                     << ", from persistent mem: " << from_persistent_mem << ", need recycle: " << need_recycle;
  //   }

  // if (IsMemoryPoolRecycle()) {
  if (false) {
    (void)mem_bufs_.insert(device_addr);
  }
  //   MS_LOG(DEBUG) << "Alloc memory details, name:" << DynamicMemAllocatorDebugInfo::GetDebugInfo().name_
  //                 << ", stream id: " << stream_id << ", address:" << device_addr << ", size:" << size
  //                 << "B, total allocated mem:" << TotalMemStatistics() << "B, peak used mem:" <<
  //                 UsedMemPeakStatistics()
  //                 << "B, in used mem:" << TotalUsedMemStatistics()
  //                 << "B, total idle mem:" << (TotalMemStatistics() - TotalUsedMemStatistics()) << "B.";
  return device_addr;
}

std::vector<DeviceMemPtr> DynamicMemPoolBestFitOpt::AllocContinuousTensorMem(const std::vector<size_t> &size_list,
                                                                             uint32_t stream_id) {
  std::vector<DeviceMemPtr> device_addr_list;
  size_t total_size = std::accumulate(size_list.begin(), size_list.end(), 0);
  // Pre-alloc the one whole piece memory.
  auto device_addr = AllocTensorMem(total_size, false, false, stream_id);
  if (!device_addr) {
    return device_addr_list;
  }

  std::lock_guard<SpinLock> lock(spin_lock_);
  // Remove the pre-alloc memory.
  auto mem_block = FindMemBlock(device_addr, common_mem_);
  if (mem_block == nullptr) {
    mem_block = FindMemBlock(device_addr, persistent_mem_);
  }
  const auto &iter = mem_block->block_all_mem_buf_map_.find(device_addr);
  if (iter == mem_block->block_all_mem_buf_map_.end()) {
    DumpDynamicMemPoolDebugInfo();
    // MS_LOG(INTERNAL_EXCEPTION) << "Can't find the device address[" << device_addr << "].";
  }
  auto mem_buf = iter->second;
  if (mem_buf->size_ < total_size) {
    DumpDynamicMemPoolDebugInfo();
    // MS_LOG(EXCEPTION) << "The size of membuf is less than total_size.";
  }
  auto rest_size = mem_buf->size_ - total_size;
  (void)mem_block->block_all_mem_buf_map_.erase(iter);
  delete mem_buf;
  // Split the pre-alloc memory into continuous memory by the size list.
  DynamicMemBufRawPtr continuous_mem_buf;
  auto buf_addr = device_addr;
  for (size_t i : size_list) {
    continuous_mem_buf = new DynamicMemBuf(buf_addr, mem_block, DynamicMemBufStatus::kMemBufUsed, i,
                                           DynamicMemAllocatorDebugInfo::GetDebugInfo().name_,
                                           DynamicMemAllocatorDebugInfo::GetDebugInfo().type_);
    (void)mem_block->block_all_mem_buf_map_.emplace(buf_addr, continuous_mem_buf);
    device_addr_list.emplace_back(buf_addr);
    buf_addr = AddressOffset(buf_addr, i);
  }
  // Update the size of the last memory buf.
  continuous_mem_buf->size_ += rest_size;
  return device_addr_list;
}

DeviceMemPtr DynamicMemPoolBestFitOpt::FindAvailableMemBuf(size_t size, bool from_persistent_mem, uint32_t stream_id) {
  auto addr = FindMemBufByStatus(size, from_persistent_mem, DynamicMemBufStatus::kMemBufIdle, stream_id);
  if (addr == nullptr && is_trigger_eager_free_) {
    // MS_LOG(DEBUG) << "Find idle mem buf failed and eager free is enabled, try to search in eager free bufs.";
    // Check total used max memory limits, since real occupy memory size equals to used mem size plus idle mem size.
    // Eager free mem may occupy some memory, so total_mem_size need multiply by a factor.
    float threshold_factor = 0.8f;
    size_t threshold = static_cast<size_t>(total_mem_size() * threshold_factor);
    if (TotalUsedMemStatistics() + TotalIdleMemStatistics() + size <= threshold) {
      addr = FindMemBufByStatus(size, from_persistent_mem, DynamicMemBufStatus::kMemBufEagerFree, stream_id);
    }
  }
  return addr;
}

DeviceMemPtr DynamicMemPoolBestFitOpt::FindMemBufByStatus(size_t size, bool from_persistent_mem,
                                                          DynamicMemBufStatus target_status, uint32_t stream_id) {
  auto addr = FindMemBufInSpecifiedMng(size, from_persistent_mem, target_status, stream_id);
  if (addr == nullptr) {
    if (from_persistent_mem && !persistent_mem_->mem_block_list_.empty()) {
      // MS_LOG(DEBUG) << "Find mem buf in current pool failed, try to find in another one.";
      addr = FindMemBufInSpecifiedMng(size, !from_persistent_mem, target_status, stream_id);
    }
  }
  return addr;
}

DeviceMemPtr DynamicMemPoolBestFitOpt::FindMemBufInSpecifiedMng(size_t size, bool from_persistent_mem,
                                                                DynamicMemBufStatus target_status, uint32_t stream_id) {
  auto &mem_mng = from_persistent_mem ? persistent_mem_ : common_mem_;
  auto &mem_buf_map = (target_status == DynamicMemBufStatus::kMemBufIdle) ? mem_mng->idle_mem_bufs_[stream_id]
                                                                          : mem_mng->eager_free_mem_bufs_[stream_id];
  auto iter = mem_buf_map.lower_bound(size);
  if (iter != mem_buf_map.end()) {
    // if (IsMemoryPoolRecycle()) {
    if (false) {
      // Ensure that the addresses corresponding to the same Tensor for each step are consistent, making the memory pool
      // recycling function more stable.
      auto find_size = iter->first;
      // Can be optimized in the future.
      auto [lb, ub] = mem_buf_map.equal_range(find_size);
      for (auto i = lb; i != ub; ++i) {
        if (i->second->device_addr_ > iter->second->device_addr_) {
          iter = i;
        }
      }
    }
    auto mem_buf = iter->second;
    if (mem_buf->status_ != target_status) {
      DumpDynamicMemPoolDebugInfo();
      //   MS_LOG(EXCEPTION) << "Mem_buf is not " << target_status << ", alloc_size[" << size << "] mem_buf_size["
      //                     << mem_buf->size_ << "] mem_buf_address[" << mem_buf->device_addr_ << "].";
    }
    mem_buf->allocator_name_ = DynamicMemAllocatorDebugInfo::GetDebugInfo().name_;
    mem_buf->allocator_type_ = DynamicMemAllocatorDebugInfo::GetDebugInfo().type_;
    // Remove map of old idle memory buf
    (void)mem_buf_map.erase(iter);
    // Divide memory buf
    if (IsSplit(size, mem_buf->size_)) {
      SplitMemBuf(size, mem_buf, mem_mng, stream_id);
    }
    mem_buf->status_ = DynamicMemBufStatus::kMemBufUsed;
    // Memory statistics
    mem_mng->mps_.total_used_mem_size_ += mem_buf->size_;
    if (mem_mng->mps_.total_used_mem_size_ > mem_mng->mps_.used_mem_peak_size_) {
      mem_mng->mps_.used_mem_peak_size_ = mem_mng->mps_.total_used_mem_size_;
    }
    if (target_status == DynamicMemBufStatus::kMemBufIdle) {
      mem_mng->mps_.total_idle_mem_size_ -= mem_buf->size_;
    } else if (target_status == DynamicMemBufStatus::kMemBufEagerFree) {
      mem_mng->mps_.total_eager_free_mem_size_ -= mem_buf->size_;
    }
    return mem_buf->device_addr_;
  }
  return nullptr;
}

size_t DynamicMemPoolBestFitOpt::MemAllocUnitSize(bool from_persistent_mem) const {
  return from_persistent_mem ? persistent_mem_->unit_size_ : common_mem_->unit_size_;
}

void DynamicMemPoolBestFitOpt::SetMemAllocUintSize(size_t common_size, size_t persist_size) {
  persistent_mem_->unit_size_ = persist_size;
  common_mem_->unit_size_ = common_size;
  config_unit_size_ = common_size;
  // MS_LOG(INFO) << "Set mem alloc unit size, common " << common_size << " persistent " << persist_size;
}

void *DynamicMemPoolBestFitOpt::GetMinUsedMemoryAddr() const {
  if (mem_bufs_.empty()) {
    return nullptr;
  }
  return *(mem_bufs_.begin());
}

void DynamicMemPoolBestFitOpt::SetMemPoolBlockSize(size_t available_device_mem_size) {
  //   auto ms_context = MsContext::GetInstance();
  //   float mem_block_size = ms_context->get_param<float>(MS_CTX_MEMPOOL_BLOCK_SIZE);
  //   if (mem_block_size == kDefaultMempoolBlockSize) {
  //     return;
  //   }

  //   size_t config_size = FloatToSize(mem_block_size * kGBToByte);
  //   if (config_size > available_device_mem_size) {
  //     // MS_LOG(WARNING) << "Memory pool block size " << config_size << " is bigger than currently available maximum
  //     // memory "
  //     //                 << available_device_mem_size << ", and the actual effective value will be "
  //     //                 << available_device_mem_size;
  //   }
  //   // Reserve 1G for persistent_mem
  //   if (available_device_mem_size > kGBToByte) {
  //     available_device_mem_size -= kGBToByte;
  //   }
  //   size_t real_block_size = std::min(config_size, available_device_mem_size);
  SetMemAllocUintSize(1 << 31);
}

DeviceMemPtr DynamicMemPoolBestFitOpt::AddMemBlockAndMemBuf(size_t size, bool from_persistent_mem, bool need_recycle,
                                                            uint32_t stream_id) {
  if (from_persistent_mem && !need_recycle && !persistent_mem_->Empty()) {
    from_persistent_mem = false;
  }

  // Try eager free routine.
  if (is_trigger_eager_free_) {
    return AddMemBlockAndMemBufByEagerFree(size, from_persistent_mem, stream_id);
  }

  size_t alloc_mem_size = CalMemBlockAllocSize(size, from_persistent_mem, need_recycle);
  // MS_LOG(DEBUG) << "CalMemBlockAllocSize return : " << size << ", alloc_mem_size : " << alloc_mem_size;
  if (alloc_mem_size == 0) {
    if (auto device_addr = FindAvailableMemBuf(size, !from_persistent_mem, stream_id)) {
      return device_addr;
    }
    if (IsEnableEagerFree()) {
      is_trigger_eager_free_ = true;
      return AddMemBlockAndMemBufByEagerFree(size, from_persistent_mem, stream_id);
    }
    return nullptr;
  }

  // Add new memory block
  DeviceMemPtr device_addr = nullptr;
  auto real_alloc_size = AllocDeviceMem(alloc_mem_size, &device_addr);
  if (real_alloc_size < size) {
    // MS_LOG(WARNING) << "Memory not enough: alloc size[" << real_alloc_size << "] is smaller than required size[" <<
    // size
    //                 << "].";
    return nullptr;
  }
  // If unit_size is changed by other function(not context), change unit_size back
  common_mem_->unit_size_ = config_unit_size_;

  return CreateMemBlockAndMemBuf(size, from_persistent_mem, device_addr, real_alloc_size,
                                 DynamicMemBufStatus::kMemBufIdle, stream_id);
}

DeviceMemPtr DynamicMemPoolBestFitOpt::AddMemBlockAndMemBufByEagerFree(size_t size, bool from_persistent_mem,
                                                                       uint32_t stream_id) {
  // Check used max memory limits.
  if (TotalUsedMemStatistics() + size > total_mem_size()) {
    // MS_LOG(ERROR) << "TotalUsedMemStatistics : " << TotalUsedMemStatistics() << " plus alloc size : " << size
    //               << " is more than total mem size : " << total_mem_size() << ".";
    return nullptr;
  }

  // MS_LOG(DEBUG) << "Try to eager free memory.";
  if (!SyncAllStreams()) {
    // MS_LOG(INTERNAL_EXCEPTION) << "Sync all streams failed.";
  }
  FreeIdleMemsByEagerFree();
  auto mem_addr = FindMemBufByStatus(size, from_persistent_mem, DynamicMemBufStatus::kMemBufEagerFree, stream_id);
  if (mem_addr != nullptr) {
    // MS_LOG(DEBUG) << "Find eager free memory success, mem_addr : " << mem_addr << ".";
    return mem_addr;
  }

  auto alloc_size = total_mem_size();
  // MS_LOG(INFO) << "Try to alloc eager free mem block, size : " << alloc_size << ".";
  DeviceMemPtr device_addr = nullptr;
  auto real_alloc_size = AllocDeviceMemByEagerFree(alloc_size, &device_addr);
  if (real_alloc_size < alloc_size) {
    // MS_LOG(ERROR) << "AllocDeviceMemByEagerFree failed, alloc_size : " << real_alloc_size << ".";
    return nullptr;
  }
  return CreateMemBlockAndMemBuf(size, from_persistent_mem, device_addr, real_alloc_size,
                                 DynamicMemBufStatus::kMemBufEagerFree, stream_id);
}

DeviceMemPtr DynamicMemPoolBestFitOpt::CreateMemBlockAndMemBuf(size_t size, bool from_persistent_mem,
                                                               DeviceMemPtr source_addr, size_t source_size,
                                                               DynamicMemBufStatus mem_buf_status, uint32_t stream_id) {
  auto mem_block = std::make_shared<DynamicMemBlock>(source_addr, source_size, stream_id);
  auto mem_mng = from_persistent_mem ? persistent_mem_ : common_mem_;
  mem_mng->AddMemBlock(mem_block, stream_id);
  // Add new memory buf.
  auto mem_buf = new DynamicMemBuf(mem_block->device_addr(), mem_block, mem_buf_status, mem_block->size(),
                                   DynamicMemAllocatorDebugInfo::GetDebugInfo().name_,
                                   DynamicMemAllocatorDebugInfo::GetDebugInfo().type_);
  // Add map of new memory buf in the block
  (void)mem_block->block_all_mem_buf_map_.emplace(mem_block->device_addr(), mem_buf);
  // Split memory buf
  if (mem_buf->size_ >= (size << 1)) {
    SplitMemBuf(size, mem_buf, mem_mng, stream_id);
  }
  mem_buf->status_ = DynamicMemBufStatus::kMemBufUsed;
  // Memory statistics
  mem_mng->mps_.total_mem_size_ += mem_block->size();
  mem_mng->mps_.total_used_mem_size_ += mem_buf->size_;
  if (mem_mng->mps_.total_used_mem_size_ > mem_mng->mps_.used_mem_peak_size_) {
    mem_mng->mps_.used_mem_peak_size_ = mem_mng->mps_.total_used_mem_size_;
  }
  //   MS_LOG(DEBUG) << "Usage: used size : " << TotalUsedMemStatistics() << ", idle size : " <<
  //   TotalIdleMemStatistics()
  //                 << ", eager free size : " << TotalEagerFreeMemStatistics() << ".";
  return mem_buf->device_addr_;
}

size_t DynamicMemPoolBestFitOpt::CalMemBlockAllocSize(size_t size, bool from_persistent_mem, bool) {
  auto device_free_mem_size = free_mem_size();
  //   if (device_free_mem_size < size && common::IsNeedProfileMemory()) {
  //     device_free_mem_size = size;
  //   }
  if (device_free_mem_size < size) {
    // MS_LOG(WARNING) << "Memory not enough: current free memory size[" << device_free_mem_size
    //                 << "] is smaller than required size[" << size << "].";
    return 0;
  }
  // The memory of the device is too small, which may cause the new application to fail.
  if (device_free_mem_size < kMinimumAllocMem) {
    // MS_LOG(WARNING) << "Device memory size [" << device_free_mem_size << "] is smaller than minimum alloc size ["
    //                 << kMinimumAllocMem << "].";
    return 0;
  }
  auto alloc_mem_size = MemAllocUnitSize(from_persistent_mem);
  // Growing at twice of alloc size
  constexpr size_t kDouble = 2;
  while (alloc_mem_size < size) {
    alloc_mem_size = alloc_mem_size * kDouble;
  }
  alloc_mem_size = std::min(alloc_mem_size, device_free_mem_size);
  return alloc_mem_size;
}

const size_t DynamicMemPoolBestFitOpt::FreeIdleMemsByEagerFree() {
  auto eager_free_mem_func = [&](MemStatusManagerPtr &mem_mng) {
    size_t free_size = 0;
    size_t real_free_size = 0;
    for (auto &idle_mem_buf : mem_mng->idle_mem_bufs_) {
      for (auto &[size, mem_buf] : idle_mem_buf) {
        free_size += size;
        real_free_size += FreeDeviceMemByEagerFree(mem_buf->device_addr_, size);
        CombineMemBuf(mem_buf->device_addr_, DynamicMemBufStatus::kMemBufIdle, DynamicMemBufStatus::kMemBufEagerFree);
      }
    }
    mem_mng->idle_mem_bufs_.clear();
    return std::make_pair(free_size, real_free_size);
  };

  const auto [persistent_free_size, persistent_real_free_size] = eager_free_mem_func(persistent_mem_);
  const auto [common_free_size, common_real_free_size] = eager_free_mem_func(common_mem_);
  auto free_size = persistent_free_size + common_free_size;
  auto real_free_size = persistent_real_free_size + common_real_free_size;
  // MS_LOG(DEBUG) << "Total eager free memory : " << free_size << ", real free : " << real_free_size << ".";
  return real_free_size;
}

void DynamicMemPoolBestFitOpt::SplitMemBuf(size_t size, const DynamicMemBufRawPtr mem_buf,
                                           const MemStatusManagerPtr &mem_mng, uint32_t stream_id) {
  const auto mem_block = mem_buf->mem_block_ptr_;
  // Divide new memory buf
  if (mem_buf->size_ < size) {
    DumpDynamicMemPoolDebugInfo();
    // MS_LOG(EXCEPTION) << "The size of membuf is less than size.";
  }
  size_t newbuf_size = mem_buf->size_ - size;
  mem_buf->size_ = size;
  DeviceMemPtr newbuf_addr = static_cast<uint8_t *>(mem_buf->device_addr_) + size;
  auto new_mem_buf = new DynamicMemBuf(newbuf_addr, mem_block, mem_buf->status_, newbuf_size);
  // auto start_time = Get();
  // Add map of new memory buf in the block
  (void)mem_block->block_all_mem_buf_map_.emplace(newbuf_addr, new_mem_buf);
  //   auto cost = Get() - start_time;
  //   std::cout << "emplace cost : " << cost << "ns" << std::endl;
  if (new_mem_buf->status_ == DynamicMemBufStatus::kMemBufIdle) {
    // Add map of new idle memory buf
    mem_mng->idle_mem_bufs_[stream_id].emplace(newbuf_size, new_mem_buf);
  } else if (new_mem_buf->status_ == DynamicMemBufStatus::kMemBufEagerFree) {
    mem_mng->eager_free_mem_bufs_[stream_id].emplace(newbuf_size, new_mem_buf);
  }
}

bool DynamicMemPoolBestFitOpt::CmpMemBlock(const DeviceMemPtr &device_addr, const DynamicMemBlockPtr &mem_block) {
  return device_addr < mem_block->device_addr();
}

DynamicMemBlockPtr DynamicMemPoolBestFitOpt::FindMemBlock(const DeviceMemPtr &device_addr,
                                                          const MemStatusManagerPtr &mem_mng) const {
  auto &&iter =
    std::upper_bound(mem_mng->mem_block_list_.begin(), mem_mng->mem_block_list_.end(), device_addr, CmpMemBlock);
  if (iter != mem_mng->mem_block_list_.begin()) {
    return *(--iter);
  }
  return nullptr;
}

void DynamicMemPoolBestFitOpt::FreeTensorMem(const DeviceMemPtr &device_addr) {
  std::lock_guard<SpinLock> lock(spin_lock_);
  FreeTensorMemInner(device_addr);
}

void DynamicMemPoolBestFitOpt::FreeTensorMemInner(const DeviceMemPtr &device_addr) {
  CombineMemBuf(device_addr, DynamicMemBufStatus::kMemBufUsed, DynamicMemBufStatus::kMemBufIdle);

  // if (IsMemoryPoolRecycle()) {
  if (false) {
    (void)mem_bufs_.erase(device_addr);
  }
  //   MS_LOG(DEBUG) << "Free memory details, name:" << DynamicMemAllocatorDebugInfo::GetDebugInfo().name_
  //                 << ", address:" << device_addr << ", total allocated mem:" << TotalMemStatistics()
  //                 << "B, peak used mem:" << UsedMemPeakStatistics() << "B, in used mem:" << TotalUsedMemStatistics()
  //                 << "B, total idle mem:" << (TotalMemStatistics() - TotalUsedMemStatistics()) << "B.";
}

void DynamicMemPoolBestFitOpt::CombineMemBuf(const DeviceMemPtr &device_addr, DynamicMemBufStatus origin_status,
                                             DynamicMemBufStatus target_status) {
  DeviceAddrMapMemBuf::iterator iter;
  MemStatusManagerPtr mem_mng = nullptr;
  // Find in the common pool.
  DynamicMemBlockPtr mem_block = FindMemBlock(device_addr, common_mem_);
  if (mem_block != nullptr) {
    iter = mem_block->block_all_mem_buf_map_.find(device_addr);
    if (iter != mem_block->block_all_mem_buf_map_.end()) {
      mem_mng = common_mem_;
    }
  }
  if (mem_mng == nullptr) {
    // Find in the persistent pool.
    mem_block = FindMemBlock(device_addr, persistent_mem_);
    if (mem_block != nullptr) {
      iter = mem_block->block_all_mem_buf_map_.find(device_addr);
      if (iter != mem_block->block_all_mem_buf_map_.end()) {
        mem_mng = persistent_mem_;
      } else {
        mem_block = nullptr;
      }
    }
  }
  if (mem_block == nullptr) {
    // Maybe destroy the memory pool first, then destroy the address, so this is normal case.
    // MS_LOG(DEBUG) << "Can't find the mem_block of the device address[" << device_addr << "].";
    return;
  }

  auto &mem_buf = iter->second;
  // if (common::IsNeedProfileMemory()) {
  //  MS_LOG(WARNING) << "Need Profile Memory, Memory pool free, total mem: " << TotalMemStatistics()
  //                  << ", peak mem: " << UsedMemPeakStatistics() << ", in use mem: " << TotalUsedMemStatistics()
  //                  << ", device address addr: " << mem_buf->device_addr_ << ", size: " << mem_buf->size_;
  //}
  if (mem_buf->status_ != origin_status) {
    DumpDynamicMemPoolDebugInfo();
    // MS_LOG(EXCEPTION) << "Find the mem_buf is not used, mem_buf_address[" << mem_buf->device_addr_ << "].";
  }
  mem_buf->status_ = target_status;
  if (origin_status == DynamicMemBufStatus::kMemBufUsed) {
    if (mem_mng->mps_.total_used_mem_size_ < mem_buf->size_) {
      DumpDynamicMemPoolDebugInfo();
      //   MS_LOG(EXCEPTION) << "The total used mem size : " << mem_mng->mps_.total_used_mem_size_
      //                     << " is less than the size of membuf : " << mem_buf->size_ << ".";
    }
    mem_mng->mps_.total_used_mem_size_ -= mem_buf->size_;
  } else if (origin_status == DynamicMemBufStatus::kMemBufIdle) {
    if (mem_mng->mps_.total_idle_mem_size_ < mem_buf->size_) {
      DumpDynamicMemPoolDebugInfo();
      //   MS_LOG(EXCEPTION) << "The total idle mem size : " << mem_mng->mps_.total_idle_mem_size_
      //                     << " is less than the size of membuf : " << mem_buf->size_ << ".";
    }
    mem_mng->mps_.total_idle_mem_size_ -= mem_buf->size_;
  } else {
    // MS_LOG(INTERNAL_EXCEPTION) << "Unsupported origin status : " << origin_status << ".";
  }

  // Combine backward(combine the next_mem_buf to mem_buf)
  auto next_iter = iter;
  (void)next_iter++;
  if (next_iter != mem_block->block_all_mem_buf_map_.end()) {
    auto next_mem_buf = next_iter->second;
    if (next_mem_buf->status_ == target_status) {
      mem_buf->size_ += next_mem_buf->size_;
      EraseMemBufByStatus(next_mem_buf->size_, next_mem_buf->device_addr_, mem_mng, target_status,
                          mem_block->stream_id_);
      (void)mem_block->block_all_mem_buf_map_.erase(next_iter);
      delete next_mem_buf;
    }
  }
  // Combine forward(combine the mem_buf to prev_mem_buf)
  CombineMemBufForward(iter, mem_block, mem_buf, mem_mng, target_status);
}

void DynamicMemPoolBestFitOpt::CombineMemBufForward(DeviceAddrMapMemBuf::iterator const &iter,
                                                    const DynamicMemBlockPtr &mem_block, DynamicMemBufRawPtr mem_buf,
                                                    const MemStatusManagerPtr &mem_mng,
                                                    DynamicMemBufStatus target_status) {
  if (iter != mem_block->block_all_mem_buf_map_.begin()) {
    auto prev_iter = iter;
    (void)prev_iter--;
    DynamicMemBufRawPtr prev_mem_buf = prev_iter->second;
    if (prev_mem_buf->status_ == target_status) {
      EraseMemBufByStatus(prev_mem_buf->size_, prev_mem_buf->device_addr_, mem_mng, target_status,
                          mem_block->stream_id_);
      prev_mem_buf->size_ += mem_buf->size_;
      (void)mem_block->block_all_mem_buf_map_.erase(iter);
      delete mem_buf;
      if (target_status == DynamicMemBufStatus::kMemBufIdle) {
        (void)mem_mng->idle_mem_bufs_[mem_block->stream_id_].emplace(prev_mem_buf->size_, prev_mem_buf);
        mem_mng->mps_.total_idle_mem_size_ += prev_mem_buf->size_;
      } else if (target_status == DynamicMemBufStatus::kMemBufEagerFree) {
        (void)mem_mng->eager_free_mem_bufs_[mem_block->stream_id_].emplace(prev_mem_buf->size_, prev_mem_buf);
        mem_mng->mps_.total_eager_free_mem_size_ += prev_mem_buf->size_;
      }
      return;
    }
  }

  if (target_status == DynamicMemBufStatus::kMemBufIdle) {
    (void)mem_mng->idle_mem_bufs_[mem_block->stream_id_].emplace(mem_buf->size_, mem_buf);
    mem_mng->mps_.total_idle_mem_size_ += mem_buf->size_;
  } else if (target_status == DynamicMemBufStatus::kMemBufEagerFree) {
    (void)mem_mng->eager_free_mem_bufs_[mem_block->stream_id_].emplace(mem_buf->size_, mem_buf);
    mem_mng->mps_.total_eager_free_mem_size_ += mem_buf->size_;
  }
}

void DynamicMemPoolBestFitOpt::EraseMemBufByStatus(size_t size, const DeviceMemPtr &device_addr,
                                                   const MemStatusManagerPtr &mem_mng,
                                                   DynamicMemBufStatus target_status, uint32_t stream_id) const {
  bool status = target_status == DynamicMemBufStatus::kMemBufIdle
                  ? mem_mng->RemoveDeviceMem(&mem_mng->idle_mem_bufs_, size, device_addr, stream_id)
                  : mem_mng->RemoveDeviceMem(&mem_mng->eager_free_mem_bufs_, size, device_addr, stream_id);
  if (!status) {
    // MS_LOG(ERROR) << "Can't find the size[" << size << "] and device address[" << device_addr
    //               << "] in the idle mem_buf.";
  }
}

void DynamicMemPoolBestFitOpt::FreePartTensorMems(const std::vector<DeviceMemPtr> &free_addrs,
                                                  const std::vector<DeviceMemPtr> &keep_addrs,
                                                  const std::vector<size_t> &keep_addr_sizes) {
  std::lock_guard<SpinLock> lock(spin_lock_);
  for (auto &free_addr : free_addrs) {
    FreeTensorMemInner(free_addr);
  }

  // MS_EXCEPTION_IF_CHECK_FAIL((keep_addrs.size() == keep_addr_sizes.size()), "The keep addrs size is wrong.");
  for (size_t i = 0; i < keep_addrs.size(); ++i) {
    KeepTensorMemByAddr(keep_addrs[i], keep_addr_sizes[i]);
  }
}

void DynamicMemPoolBestFitOpt::KeepTensorMemByAddr(const DeviceMemPtr &device_addr, size_t size) {
  // Fetch the memblock and membuf by the device address.
  auto [mem_block, mem_buf, mem_mng] = FindByKeepAddr(device_addr);
  if (mem_buf->status_ != DynamicMemBufStatus::kMemBufIdle) {
    DumpDynamicMemPoolDebugInfo();
    // MS_LOG(EXCEPTION) << "The membuf status isn't idle for addr:" << device_addr << ", size:" << size
    //                   << ", find the mem buf addr:" << mem_buf->device_addr_ << ", size:" << mem_buf->size_;
  }

  // Calculate the size of left and right split membuf.
  size_t split_left_size = CalAddressOffset(device_addr, mem_buf->device_addr_);
  // MS_EXCEPTION_IF_CHECK_FAIL((mem_buf->size_ >= (split_left_size + size)), "The split size is wrong.");
  size_t split_right_szie = mem_buf->size_ - split_left_size - size;

  // Split the left membuf.
  EraseMemBufByStatus(mem_buf->size_, mem_buf->device_addr_, mem_mng, mem_buf->status_, mem_block->stream_id_);
  if (split_left_size == 0) {
    mem_buf->status_ = DynamicMemBufStatus::kMemBufUsed;
    mem_buf->size_ = size;
    mem_buf->allocator_name_ = DynamicMemAllocatorDebugInfo::GetDebugInfo().name_;
    mem_buf->allocator_type_ = DynamicMemAllocatorDebugInfo::GetDebugInfo().type_;
  } else {
    mem_buf->size_ = split_left_size;
    (void)mem_mng->idle_mem_bufs_[mem_block->stream_id_].emplace(mem_buf->size_, mem_buf);

    auto used_mem_buf = new DynamicMemBuf(device_addr, mem_block, DynamicMemBufStatus::kMemBufUsed, size,
                                          DynamicMemAllocatorDebugInfo::GetDebugInfo().name_,
                                          DynamicMemAllocatorDebugInfo::GetDebugInfo().type_);
    (void)mem_block->block_all_mem_buf_map_.emplace(device_addr, used_mem_buf);
  }

  // Split the right membuf.
  if (split_right_szie > 0) {
    DeviceMemPtr right_buf_addr = AddressOffset(device_addr, size);
    auto right_mem_buf =
      new DynamicMemBuf(right_buf_addr, mem_block, DynamicMemBufStatus::kMemBufIdle, split_right_szie);
    (void)mem_block->block_all_mem_buf_map_.emplace(right_buf_addr, right_mem_buf);
    (void)mem_mng->idle_mem_bufs_[mem_block->stream_id_].emplace(right_mem_buf->size_, right_mem_buf);
  }

  // Memory statistics.
  mem_mng->mps_.total_used_mem_size_ += size;
  if (mem_mng->mps_.total_used_mem_size_ > mem_mng->mps_.used_mem_peak_size_) {
    mem_mng->mps_.used_mem_peak_size_ = mem_mng->mps_.total_used_mem_size_;
  }
  mem_mng->mps_.total_idle_mem_size_ -= size;
  //   MS_LOG(DEBUG) << "Keep memory details, name:" << DynamicMemAllocatorDebugInfo::GetDebugInfo().name_
  //                 << ", address:" << device_addr << ", size:" << size << "B, total allocated mem:" <<
  //                 TotalMemStatistics()
  //                 << "B, peak used mem:" << UsedMemPeakStatistics() << "B, in used mem:" << TotalUsedMemStatistics()
  //                 << "B, total idle mem:" << (TotalMemStatistics() - TotalUsedMemStatistics()) << "B.";
}

DynamicMemBufRawPtr DynamicMemPoolBestFitOpt::FindMemBufByKeepAddr(const DeviceMemPtr &device_addr,
                                                                   const DynamicMemBlockPtr &mem_block) const {
  auto &&iter = mem_block->block_all_mem_buf_map_.upper_bound(device_addr);
  if (iter != mem_block->block_all_mem_buf_map_.begin()) {
    return (--iter)->second;
  }
  return nullptr;
}

std::tuple<DynamicMemBlockPtr, DynamicMemBufRawPtr, MemStatusManagerPtr> DynamicMemPoolBestFitOpt::FindByKeepAddr(
  const DeviceMemPtr &device_addr) const {
  auto is_addr_in_membuf = [](const DeviceMemPtr &device_addr, const DynamicMemBufRawPtr mem_buf) {
    return (mem_buf != nullptr) && (device_addr >= mem_buf->device_addr_) &&
           (mem_buf->size_ >= CalAddressOffset(device_addr, mem_buf->device_addr_));
  };

  // Find in the common pool.
  auto mem_block = FindMemBlock(device_addr, common_mem_);
  if (mem_block != nullptr) {
    auto mem_buf = FindMemBufByKeepAddr(device_addr, mem_block);
    if (is_addr_in_membuf(device_addr, mem_buf)) {
      return std::make_tuple(mem_block, mem_buf, common_mem_);
    }
  }

  // Find in the persistent pool.
  mem_block = FindMemBlock(device_addr, persistent_mem_);
  if (mem_block != nullptr) {
    auto mem_buf = FindMemBufByKeepAddr(device_addr, mem_block);
    if (is_addr_in_membuf(device_addr, mem_buf)) {
      return std::make_tuple(mem_block, mem_buf, persistent_mem_);
    }
  }

  return std::make_tuple(nullptr, nullptr, common_mem_);
}

void DynamicMemPoolBestFitOpt::ReleaseDeviceRes() {
  std::lock_guard<SpinLock> lock(spin_lock_);

  DumpDynamicMemPoolStateInfo();

  auto fn = [this](const MemStatusManagerPtr &mem_mng) {
    for (auto &iter : mem_mng->mem_block_list_) {
      auto &device_addr = iter->device_addr_base_;
      if (device_addr != nullptr) {
        if (!FreeDeviceMem(device_addr)) {
          // MS_LOG(ERROR) << "Free device memory[" << device_addr << "] error.";
        }
        device_addr = nullptr;
      }
    }
    mem_mng->Clear();
  };
  fn(common_mem_);
  fn(persistent_mem_);
}

void DynamicMemPoolBestFitOpt::DumpDynamicMemPoolStateInfo() {
  //   size_t total_used_size_list[kAllocatorTypeNum] = {0};
  //   auto fn = [&](const MemStatusManagerPtr &mem_mng, const std::string &mem_type) {
  //     if (mem_mng->Empty()) {
  //       return;
  //     }

  //     std::ostringstream buf;
  //     for (size_t i = 0; i < mem_mng->mem_block_list_.size(); ++i) {
  //       size_t mem_block_used_size = 0;
  //       for (auto mb = mem_mng->mem_block_list_[i]->block_all_mem_buf_map_.begin();
  //            mb != mem_mng->mem_block_list_[i]->block_all_mem_buf_map_.end(); ++mb) {
  //         if (mb->second->status_ == DynamicMemBufStatus::kMemBufUsed) {
  //           mem_block_used_size += mb->second->size_;
  //           MS_EXCEPTION_IF_CHECK_FAIL((static_cast<int>(mb->second->allocator_type_) < kAllocatorTypeNum),
  //                                      "Allocator type is out of range.");
  //           total_used_size_list[static_cast<int>(mb->second->allocator_type_)] += mb->second->size_;
  //         }
  //       }
  //       buf << ", block[" << i << "] block size:" << mem_mng->mem_block_list_[i]->mem_block_size_ / kMBToByte
  //           << "M idle size:" << (mem_mng->mem_block_list_[i]->mem_block_size_ - mem_block_used_size) / kMBToByte <<
  //           "M";
  //     }

  //     // Dump all the memory buf info
  //     MS_LOG(INFO) << mem_type << " pool info: Total allocated mem:" << mem_mng->mps_.total_mem_size_ / kMBToByte
  //                  << "M, peak used mem:" << mem_mng->mps_.used_mem_peak_size_ / kMBToByte
  //                  << "M, in used mem:" << mem_mng->mps_.total_used_mem_size_ / kMBToByte << "M, total idle mem:"
  //                  << (mem_mng->mps_.total_mem_size_ - mem_mng->mps_.total_used_mem_size_) / kMBToByte
  //                  << "M. Block unit size:" << mem_mng->unit_size_ / kMBToByte
  //                  << "M, block counts:" << mem_mng->mem_block_list_.size() << buf.str();
  //   };

  //   fn(common_mem_, std::string(kCommonMem));
  //   fn(persistent_mem_, std::string(kPersistentParamMem));
  //   MS_LOG(INFO) << "The dynamic memory pool total allocated mem:" << TotalMemStatistics() / kMBToByte
  //                << "M, peak used mem:" << UsedMemPeakStatistics() / kMBToByte
  //                << "M, in used mem:" << TotalUsedMemStatistics() / kMBToByte
  //                << "M, total idle mem:" << TotalIdleMemStatistics() / kMBToByte
  //                << "M, total eager free mem:" << TotalEagerFreeMemStatistics() / kMBToByte
  //                << "M. Weight used size:" << total_used_size_list[static_cast<int>(AllocatorType::kWeight)] /
  //                kMBToByte
  //                << "M, constant value used size:"
  //                << total_used_size_list[static_cast<int>(AllocatorType::kConstantValue)] / kMBToByte
  //                << "M, kernel output used size:"
  //                << total_used_size_list[static_cast<int>(AllocatorType::kKernelOutput)] / kMBToByte
  //                << "M, other used size:" << total_used_size_list[static_cast<int>(AllocatorType::kOther)] /
  //                kMBToByte
  //                << "M.";
}

void DynamicMemPoolBestFitOpt::DumpDynamicMemPoolDebugInfo() {
  //   auto fn = [](const MemStatusManagerPtr &mem_mng, const std::string &mem_type) {
  //     MS_EXCEPTION_IF_NULL(mem_mng);
  //     size_t total_mem = 0;
  //     size_t total_used_mem = 0;
  //     size_t total_idle_mem1 = 0;
  //     size_t total_idle_mem2 = 0;
  //     size_t total_eager_free_mem = 0;
  //     // Dump the memory block info and memory buf info.
  //     MS_LOG(WARNING) << mem_type << " all mem_block info: counts[" << mem_mng->mem_block_list_.size() << "].";
  //     for (auto iter = mem_mng->mem_block_list_.begin(); iter != mem_mng->mem_block_list_.end(); ++iter) {
  //       total_mem += (*iter)->size();
  //       auto mem_buf_map = (*iter)->block_all_mem_buf_map_;
  //       MS_LOG(WARNING) << " MemBlock info: number[" << iter - mem_mng->mem_block_list_.begin() << "]
  //       mem_buf_counts["
  //                       << mem_buf_map.size() << "] base_address[" << (*iter)->device_addr() << "] block_size["
  //                       << (*iter)->size() << "].";
  //       for (auto iter_mem_buf = mem_buf_map.begin(); iter_mem_buf != mem_buf_map.end(); ++iter_mem_buf) {
  //         auto mem_buf = iter_mem_buf->second;
  //         MS_EXCEPTION_IF_NULL(mem_buf);
  //         if (mem_buf->status_ == DynamicMemBufStatus::kMemBufIdle) {
  //           total_idle_mem1 += mem_buf->size_;
  //         } else if (mem_buf->status_ == DynamicMemBufStatus::kMemBufUsed) {
  //           total_used_mem += mem_buf->size_;
  //         } else if (mem_buf->status_ == DynamicMemBufStatus::kMemBufEagerFree) {
  //           total_eager_free_mem += mem_buf->size_;
  //         }
  //         MS_LOG(INFO) << "  MemBuf info: address[" << mem_buf->device_addr_ << "] size[" << mem_buf->size_ << "]
  //         status["
  //                      << kBufStatusString.at(mem_buf->status_) << "] name["
  //                      << (mem_buf->allocator_name_.empty() ? "Unknown" : mem_buf->allocator_name_) << "] type["
  //                      << kAllocatorTypeString.at(mem_buf->allocator_type_) << "].";
  //       }
  //     }
  //     // Dump all the idle memory buf info.
  //     MS_LOG(WARNING) << mem_type << " all idle_mem_bufs info: counts[" << mem_mng->idle_mem_bufs_.size() << "].";
  //     for (size_t stream_id = 0; stream_id < mem_mng->idle_mem_bufs_.size(); stream_id++) {
  //       const auto &idle_mem_buf = mem_mng->idle_mem_bufs_[stream_id];
  //       if (idle_mem_buf.empty()) {
  //         continue;
  //       }
  //       MS_LOG(WARNING) << "  stream id : " << stream_id << ", idle mem buf info : count[]" << idle_mem_buf.size()
  //                       << "].";
  //       for (auto iter_idle = idle_mem_buf.begin(); iter_idle != idle_mem_buf.end(); ++iter_idle) {
  //         auto mem_buf = iter_idle->second;
  //         MS_EXCEPTION_IF_NULL(mem_buf);
  //         total_idle_mem2 += mem_buf->size_;
  //         MS_LOG(INFO) << " Idle mem_buf info: size[" << mem_buf->size_ << "] address[" << mem_buf->device_addr_
  //                      << "] status[" << kBufStatusString.at(mem_buf->status_) << "].";
  //       }
  //     }
  //     // Dump all the eager free memory buf info.
  //     size_t total_eager_free_mem_in_map = 0;
  //     MS_LOG(WARNING) << mem_type << " all eager free mem_buf info: counts[" << mem_mng->eager_free_mem_bufs_.size()
  //                     << "].";
  //     for (size_t stream_id = 0; stream_id < mem_mng->eager_free_mem_bufs_.size(); stream_id++) {
  //       const auto &eager_free_mem_buf = mem_mng->eager_free_mem_bufs_[stream_id];
  //       MS_LOG(WARNING) << "  stream id : " << stream_id << ", idle mem buf info : count[]" <<
  //       eager_free_mem_buf.size()
  //                       << "].";
  //       for (auto iter = eager_free_mem_buf.begin(); iter != eager_free_mem_buf.end(); ++iter) {
  //         auto mem_buf = iter->second;
  //         MS_EXCEPTION_IF_NULL(mem_buf);
  //         total_eager_free_mem_in_map += mem_buf->size_;
  //         MS_LOG(INFO) << " Eager free mem_buf info: size[" << mem_buf->size_ << "] address[" <<
  //         mem_buf->device_addr_
  //                      << "] status[" << kBufStatusString.at(mem_buf->status_) << "].";
  //       }
  //     }
  //     // Dump the memory statistical info.
  //     MS_LOG(WARNING) << mem_type << " total allocated memory[" << total_mem << "], used memory[" << total_used_mem
  //                     << "], idle memory[" << total_idle_mem1 << "].";
  //     if (total_idle_mem1 != total_idle_mem2) {
  //       MS_LOG(ERROR) << "Check error: the idle memory in the mem_block is not equal the global idle memory.";
  //     }
  //     if (total_eager_free_mem != total_eager_free_mem_in_map) {
  //       MS_LOG(ERROR) << "Check error: the eager free memory in the mem_block is not equal the global eager free
  //       memory.";
  //     }
  //     if (total_mem != total_used_mem + total_idle_mem1 + total_eager_free_mem) {
  //       MS_LOG(ERROR) << "Check error: the the total memory : " << total_mem
  //                     << " is not equal the sum of used memory : " << total_used_mem
  //                     << ", idle memory : " << total_idle_mem1 << " and eager free memory : " << total_eager_free_mem
  //                     << ".";
  //     }
  //   };

  //   MS_LOG(WARNING) << "Start dump dynamic memory pool debug info.";
  //   fn(common_mem_, std::string(kCommonMem));
  //   fn(persistent_mem_, std::string(kPersistentParamMem));
  //   MS_LOG(WARNING) << "Finish dump dynamic memory pool debug info.";
}

// The statistics information.
size_t DynamicMemPoolBestFitOpt::TotalMemStatistics() const {
  return common_mem_->mps_.total_mem_size_ + persistent_mem_->mps_.total_mem_size_;
}
size_t DynamicMemPoolBestFitOpt::TotalUsedMemStatistics() const {
  return common_mem_->mps_.total_used_mem_size_ + persistent_mem_->mps_.total_used_mem_size_;
}
size_t DynamicMemPoolBestFitOpt::TotalIdleMemStatistics() const {
  return common_mem_->mps_.total_idle_mem_size_ + persistent_mem_->mps_.total_idle_mem_size_;
}
size_t DynamicMemPoolBestFitOpt::TotalEagerFreeMemStatistics() const {
  return common_mem_->mps_.total_eager_free_mem_size_ + persistent_mem_->mps_.total_eager_free_mem_size_;
}
size_t DynamicMemPoolBestFitOpt::UsedMemPeakStatistics() const {
  return common_mem_->mps_.used_mem_peak_size_ + persistent_mem_->mps_.used_mem_peak_size_;
}

void MemStatusManager::AddMemBlock(const DynamicMemBlockPtr &mem_block, uint32_t stream_id) {
  mem_blocks_[stream_id].emplace_back(mem_block);

  auto iter = std::upper_bound(mem_block_list_.begin(), mem_block_list_.end(), mem_block->device_addr(),
                               [](const DeviceMemPtr &device_addr, const DynamicMemBlockPtr &mem_block) {
                                 return device_addr < mem_block->device_addr();
                               });
  (void)mem_block_list_.insert(iter, mem_block);
}

bool MemStatusManager::RemoveDeviceMem(std::vector<SizeMapMemBuf> *container, const size_t size,
                                       const DeviceMemPtr &device_addr, uint32_t stream_id) {
  auto &mem_buf_map = (*container)[stream_id];
  auto &&iter = mem_buf_map.equal_range(size);
  while (iter.first != iter.second) {
    if (iter.first->second->device_addr_ == device_addr) {
      (void)mem_buf_map.erase(iter.first);
      return true;
    }
    (void)iter.first++;
  }
  return false;
}

void MemStatusManager::Clear() noexcept {
  mem_blocks_.clear();
  mem_block_list_.clear();
  idle_mem_bufs_.clear();
  eager_free_mem_bufs_.clear();
}
