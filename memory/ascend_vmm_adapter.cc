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
#include "ascend_vmm_adapter.h"
#include <map>
#include <vector>
#include <tuple>

// #include "utils/convert_utils_base.h"
// #include "utils/ms_utils.h"
// #include "transform/symbol/symbol_utils.h"
// #include "transform/symbol/acl_rt_symbol.h"

static VmmManager vmm_manager_(kMaxUsedMemSize);

size_t AscendVmmAdapter::GetRoundUpAlignSize(size_t input_size) const {
  return ((input_size + kVmmAlignSize - 1) / kVmmAlignSize) * kVmmAlignSize;
}

size_t AscendVmmAdapter::GetRoundDownAlignSize(size_t input_size) const {
  return (input_size / kVmmAlignSize) * kVmmAlignSize;
}

size_t AscendVmmAdapter::GetHandleSize(size_t input_size) {
  if (input_size % kVmmAlignSize != 0) {
    throw std::runtime_error("Input size must be multiple of 2MB, but got : " + std::to_string(input_size));
    // MS_LOG(EXCEPTION) << "Input size must be multiple of 2MB, but got " << input_size;
  }
  return input_size / kVmmAlignSize;
}

DeviceMemPtr AscendVmmAdapter::FindVmmSegment(const DeviceMemPtr addr) {
  auto it = vmm_map_.upper_bound(addr);
  if (it == vmm_map_.begin()) {
    return nullptr;
  } else {
    --it;
    return it->first;
  }
  return nullptr;
}

void AscendVmmAdapter::ClearAllMemory() {
  for (auto &kv : vmm_map_) {
    if (kv.second == nullptr) {
      continue;
    }
    // auto ret = CALL_ASCEND_API(aclrtUnmapMem, kv.first);
    // if (ret != ACL_ERROR_NONE) {
    //   MS_LOG(ERROR) << "Unmap memory failed.";
    // }
    auto ret = vmm_manager_.Unmap(kv.first);
    if (!ret) {
      std::cout << "unmap failed" << std::endl;
    }

    // ret = CALL_ASCEND_API(aclrtFreePhysical, kv.second);
    // if (ret != ACL_ERROR_NONE) {
    //   MS_LOG(ERROR) << "Free physical memory failed.";
    // }
    ret = vmm_manager_.FreeHandle(kv.second);
    if (!ret) {
      std::cout << "free handle failed" << std::endl;
    }
  }
  while (!cached_handle_sets_.empty()) {
    auto handle = *cached_handle_sets_.begin();
    cached_handle_sets_.erase(cached_handle_sets_.begin());
    // auto ret = CALL_ASCEND_API(aclrtFreePhysical, handle);
    // if (ret != ACL_ERROR_NONE) {
    //   MS_LOG(ERROR) << "Free physical memory failed.";
    // }
    auto ret = vmm_manager_.FreeHandle(handle);
    if (!ret) {
      std::cout << "free handle failed" << std::endl;
    }
  }
  for (auto &addr : all_reserve_mems_) {
    // CALL_ASCEND_API(aclrtReleaseMemAddress, addr);
    vmm_manager_.FreeVirtualAddress(addr);
  }
  all_reserve_mems_.clear();
  vmm_map_.clear();
}

AscendVmmAdapter::~AscendVmmAdapter() { ClearAllMemory(); }

void MoveBackMappedHandle(std::map<DeviceMemPtr, aclrtDrvMemHandle> *mapped_vmm_handle,
                          std::map<DeviceMemPtr, aclrtDrvMemHandle> *vmm_map,
                          std::set<aclrtDrvMemHandle> *cached_handle_sets_) {
  for (const auto [address, handle] : *mapped_vmm_handle) {
    // auto ret = CALL_ASCEND_API(aclrtUnmapMem, address);
    auto ret = vmm_manager_.Unmap(address);
    if (!ret) {
      std::cout << "Unmap memory failed, address : " << address << "." << std::endl;
    }
    // if (ret != ACL_ERROR_NONE) {
    // MS_LOG(ERROR) << "Unmap memory failed, address : " << address << ".";
    // }
    else {
      auto iter = vmm_map->find(address);
      if (iter == vmm_map->end()) {
        // MS_LOG(ERROR) << "Find vmm map address : " << address << " failed.";
        std::cout << "Find vmm map address : " << address << " failed." << std::endl;
      } else {
        iter->second = nullptr;
        cached_handle_sets_->insert(handle);
      }
    }
  }
}

inline size_t CalAddressOffset(void *dst_address, void *ori_address) {
  return static_cast<uint8_t *>(dst_address) - static_cast<uint8_t *>(ori_address);
}

inline uint8_t *AddressOffset(void *address, size_t offset) { return static_cast<uint8_t *>(address) + offset; }

size_t AscendVmmAdapter::MmapDeviceMem(const size_t size, const DeviceMemPtr addr, const size_t max_size) {
  //   if (common::IsNeedProfileMemory()) {
  //     MS_LOG(EXCEPTION) << "VMM is not supported in dry run mode.";
  //   }
  //   MS_EXCEPTION_IF_NULL(addr);
  //   MS_LOG(DEBUG) << "VMM MmapDeviceMem size:" << size << ", addr:" << addr
  //                 << ", cached_handle_sets_ size : " << cached_handle_sets_.size() << ".";
  //   auto context = MsContext::GetInstance();
  //   MS_EXCEPTION_IF_NULL(context);
  //   auto device_id = context->get_param<uint32_t>(MS_CTX_DEVICE_ID);

  auto vmm_start_addr = FindVmmSegment(addr);
  if (vmm_start_addr == nullptr) {
    // MS_LOG(ERROR) << "Can not find the vmm segment.";
    std::cout << "Can not find the vmm segment." << std::endl;
    return 0;
  }

  //   aclrtPhysicalMemProp prop = {};
  //   prop.handleType = ACL_MEM_HANDLE_TYPE_NONE;
  //   prop.allocationType = ACL_MEM_ALLOCATION_TYPE_PINNED;
  //   prop.memAttr = ACL_HBM_MEM_HUGE;
  //   prop.location.type = ACL_MEM_LOCATION_TYPE_DEVICE;
  //   prop.location.id = device_id;
  //   prop.reserve = 0;
  auto start_offset = CalAddressOffset(addr, vmm_start_addr);
  auto align_size = GetRoundUpAlignSize(size + start_offset);
  auto handle_size = GetHandleSize(align_size);
  auto iter = vmm_map_.find(vmm_start_addr);

  std::map<DeviceMemPtr, aclrtDrvMemHandle> mapped_vmm_handle;
  for (size_t i = 0; i < handle_size; ++i) {
    auto new_addr = AddressOffset(vmm_start_addr, i * kVmmAlignSize);
    if (iter == vmm_map_.end() || iter->first != new_addr) {
      // MS_LOG(ERROR) << "Can not find the vmm segment.";
      std::cout << "Can not find the vmm segment." << std::endl;
      return 0;
    }
    if (iter->second != nullptr) {
      iter++;
      continue;
    }
    aclrtDrvMemHandle handle = nullptr;
    if (!cached_handle_sets_.empty()) {
      handle = *cached_handle_sets_.begin();
      cached_handle_sets_.erase(cached_handle_sets_.begin());
    } else {
      if (physical_handle_size_ * kVmmAlignSize >= max_size) {
        // MS_LOG(INFO) << "Mapped too much memory, physical_handle_size_ : " << physical_handle_size_
        //              << ", max_size : " << max_size << ", addr : " << addr << ", size : " << size << ".";
        std::cout << "Mapped too much memory, physical_handle_size_ : " << physical_handle_size_
                  << ", max_size : " << max_size << ", addr : " << addr << ", size : " << size << "." << std::endl;
        MoveBackMappedHandle(&mapped_vmm_handle, &vmm_map_, &cached_handle_sets_);
        return 0;
      }

      // auto ret = CALL_ASCEND_API(aclrtMallocPhysical, &handle, kVmmAlignSize, &prop, 0);
      //   if (ret != ACL_ERROR_NONE) {
      //     size_t used_handle_size = 0;
      //     for (const auto &[k, v] : vmm_map_) {
      //       if (v != nullptr) {
      //         MS_LOG(DEBUG) << "Inuse handle address : " << k << ", handle : " << v << ".";
      //         used_handle_size += 1;
      //       }
      //     }
      //     used_handle_size += cached_handle_sets_.size();
      //     // This may be a normal case at the memory usage boundary.
      //     MS_LOG(WARNING) << "Malloc physical memory failed, inuse physical memory handle size : " <<
      //     used_handle_size
      //                     << ", physical_handle_size_ size : " << physical_handle_size_ << ".";
      //     MoveBackMappedHandle(&mapped_vmm_handle, &vmm_map_, &cached_handle_sets_);
      //     return 0;
      //   } else {
      //     physical_handle_size_++;
      //     if (physical_handle_size_ * kVmmAlignSize >= max_size) {
      //       MS_LOG(WARNING) << "Mapped too much memory, physical_handle_size_ : " << physical_handle_size_
      //                       << ", max_size : " << max_size << ".";
      //     }
      //   }
      handle = vmm_manager_.GetHandle();
      if (handle == nullptr) {
        size_t used_handle_size = 0;
        for (const auto &[k, v] : vmm_map_) {
          if (v != nullptr) {
            // MS_LOG(DEBUG) << "Inuse handle address : " << k << ", handle : " << v << ".";
            used_handle_size += 1;
          }
        }
        used_handle_size += cached_handle_sets_.size();
        // This may be a normal case at the memory usage boundary.
        std::cout << "Malloc physical memory failed, inuse physical memory handle size : " << used_handle_size
                  << ", physical_handle_size_ size : " << physical_handle_size_ << "." << std::endl;
        MoveBackMappedHandle(&mapped_vmm_handle, &vmm_map_, &cached_handle_sets_);
        return 0;
      } else {
        physical_handle_size_++;
        if (physical_handle_size_ * kVmmAlignSize >= max_size) {
          std::cout << "Mapped too much memory, physical_handle_size_ : " << physical_handle_size_
                    << ", max_size : " << max_size << "." << std::endl;
        }
      }
    }

    // auto ret = CALL_ASCEND_API(aclrtMapMem, new_addr, kVmmAlignSize, 0, handle, 0);
    auto ret = vmm_manager_.Map(handle, new_addr);
    if (!ret) {
      std::cout << "Map memory failed." << std::endl;
      cached_handle_sets_.insert(handle);
      MoveBackMappedHandle(&mapped_vmm_handle, &vmm_map_, &cached_handle_sets_);
      return 0;
    }
    mapped_vmm_handle[iter->first] = handle;
    iter->second = handle;
    iter++;
  }

  //   static bool enable_trace_mem = common::IsEnableAllocConfig(common::kAllocMemoryTracker);
  //   if (enable_trace_mem) {
  //     MS_LOG(INFO) << "Total physical memory handle size : " << physical_handle_size_ << ".";
  //   }
  return size;
}

size_t AscendVmmAdapter::AllocDeviceMem(size_t size, DeviceMemPtr *addr) {
  // MS_EXCEPTION_IF_NULL(addr);
  size_t align_size = GetRoundUpAlignSize(size);
  std::cout << "VMM AllocDeviceMem size:" << size << ", align_size:" << align_size << std::endl;
  // auto ret = CALL_ASCEND_API(aclrtReserveMemAddress, addr, align_size, 0, nullptr, 1);
  // if (ret != ACL_ERROR_NONE) {
  //   MS_LOG(ERROR) << "Reserve memory address failed.";
  //  return 0;
  // }
  auto virtual_addresss = vmm_manager_.MallocVirtualAddress(align_size);
  if (virtual_addresss == nullptr) {
    std::cout << "Reserve memory address failed." << std::endl;
    return 0;
  }
  *addr = virtual_addresss->addr_;

  all_reserve_mems_.push_back(*addr);
  auto handle_size = GetHandleSize(align_size);
  for (size_t i = 0; i < handle_size; i++) {
    auto new_addr = AddressOffset(*addr, i * kVmmAlignSize);
    vmm_map_[new_addr] = nullptr;
  }
  return align_size;
}

size_t AscendVmmAdapter::EagerFreeDeviceMem(const DeviceMemPtr addr, const size_t size) {
  //   if (common::IsNeedProfileMemory()) {
  //     MS_LOG(EXCEPTION) << "VMM is not supported in dry run mode.";
  //   }
  // MS_LOG(DEBUG) << "Eager free device mem addr :" << addr << ", size :" << size
  //               << ", cached_handle_sets_ size : " << cached_handle_sets_.size() << ".";
  size_t ret_size = 0;
  auto iter = vmm_map_.lower_bound(addr);
  if (iter == vmm_map_.end()) {
    // Memory less than 2MB may be at the end of a vmm segment, and it's a normal case.
    if (size >= kVmmAlignSize) {
      std::cout << "Can not find the vmm segment." << std::endl;
    }
    return 0;
  }
  auto vmm_start_addr = iter->first;
  auto free_end_addr = AddressOffset(addr, size);
  while (true) {
    auto vmm_end_addr = AddressOffset(vmm_start_addr, kVmmAlignSize);
    if (vmm_end_addr > free_end_addr) {
      break;
    }
    if (iter == vmm_map_.end() || iter->first != vmm_start_addr) {
      std::cout << "Can not find the vmm segment." << std::endl;
      return 0;
    }
    if (iter->second == nullptr) {
      iter++;
      vmm_start_addr = vmm_end_addr;
      // Here nullptr may be huge, skip do logging.
      continue;
    }
    // auto ret = CALL_ASCEND_API(aclrtUnmapMem, vmm_start_addr);
    // if (ret != ACL_ERROR_NONE) {
    //   MS_LOG(ERROR) << "Unmap memory failed.";
    //   return 0;
    // }
    auto ret = vmm_manager_.Unmap(vmm_start_addr);
    if (!ret) {
      std::cout << "Unmap memory failed" << std::endl;
      return 0;
    }

    cached_handle_sets_.insert(iter->second);
    iter->second = nullptr;
    iter++;
    vmm_start_addr = vmm_end_addr;
    ret_size += kVmmAlignSize;
  }
  std::cout << "After eager free, cached_handle_sets_ size : " << cached_handle_sets_.size()
            << ", expected free size : " << size << ", real size : " << ret_size << "." << std::endl;
  return ret_size;
}