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

#include "dynamic_memory_pool.h"

#include <stdlib.h>

#include <atomic>
#include <memory>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <iostream>

constexpr size_t kMbByte = 1 << 20;
constexpr size_t kDefaultMapSize = kMbByte * 2;

// support
//    1 alloc virtual address
//    2 map virtual address
//    3 unmap virtual address
// need to check
//    1 virtual address support valid method to check address has beed mapped
struct VirtualAddress {
  VirtualAddress(void *addr, size_t size) : addr_(addr), size_(size) {}
  ~VirtualAddress() {}

  void *addr_;
  size_t size_;
};

struct MemHandle {
  void Map(void *addr, size_t size = kDefaultMapSize) {
    mapped_ = true;
    addr_ = addr;
    size_ = size;
  }

  void Unmap(void *addr) {
    if (addr_ != addr) {
      throw std::runtime_error("Unmap failed.");
    }

    mapped_ = false;
    addr_ = nullptr;
    size_ = 0;
  }

  bool mapped_{false};
  void *addr_{nullptr};
  size_t size_{0};
};

struct Comparator {
  bool operator()(const void *l, const void *r) const {
    return reinterpret_cast<size_t>(l) < reinterpret_cast<size_t>(r);
  }
};

struct VmmManager {
  VmmManager(size_t max_mem_size) : max_mem_size_(max_mem_size) {}
  ~VmmManager() {
    mapped_addrs_.clear();
    for (auto &virtual_address : virtual_addresses_) {
      delete virtual_address.second;
    }
    virtual_addresses_.clear();
    for (auto handle : handles_) {
      delete handle;
    }
    handles_.clear();
  }

  VirtualAddress *MallocVirtualAddress(size_t size) {
    void *addr = reinterpret_cast<void *>(base_addr_);
    base_addr_ += size;
    VirtualAddress *virtual_address = new VirtualAddress(addr, size);
    virtual_addresses_.emplace(addr, virtual_address);
    return virtual_address;
  }

  bool FreeVirtualAddress(void *addr) {
    auto iter = virtual_addresses_.find(addr);
    if (iter == virtual_addresses_.end()) {
      throw std::runtime_error("free virtual address failed.");
      return false;
    }
    delete iter->second;
    virtual_addresses_.erase(iter);
    return true;
  }

  MemHandle *GetHandle() {
    if (handles_.size() * kDefaultMapSize >= max_mem_size_) {
      std::cout << "Get handle failed." << std::endl;
      return nullptr;
    }

    MemHandle *handle = new MemHandle();
    handles_.emplace(handle);
    return handle;
  }

  bool FreeHandle(MemHandle *handle) {
    auto iter = handles_.find(handle);
    if (iter == handles_.end()) {
      return false;
    }
    handles_.erase(iter);
    delete handle;
    return true;
  }

  bool Map(MemHandle *handle, void *addr) {
    // check first
    auto &&iter = mapped_addrs_.find(addr);
    if (iter != mapped_addrs_.end()) {
      throw std::runtime_error("map failed, addr is already exist.");
    }
    size_t map_addr_size = reinterpret_cast<size_t>(addr);
    auto &&upper_iter = mapped_addrs_.upper_bound(addr);
    if (upper_iter != mapped_addrs_.end()) {
      size_t addr_size = reinterpret_cast<size_t>(upper_iter->first);
      if (addr_size < map_addr_size + kDefaultMapSize) {
        throw std::runtime_error("map failed up check, addr : " + std::to_string(map_addr_size) +
                                 ", exist addr : " + std::to_string(addr_size));
      }
      if (upper_iter != mapped_addrs_.begin()) {
        auto low_iter = --upper_iter;
        addr_size = reinterpret_cast<size_t>(low_iter->first);
        if (addr_size + kDefaultMapSize > map_addr_size) {
          throw std::runtime_error("map failed low check, addr : " + std::to_string(map_addr_size) +
                                   ", exist addr : " + std::to_string(addr_size));
        }
      }
    }

    handle->Map(addr);
    mapped_addrs_.emplace(addr, handle);
    std::cout << "Map addr : " << addr << " - " << std::to_string(reinterpret_cast<size_t>(addr)) << std::endl;
    return true;
  }

  bool Unmap(void *addr) {
    auto &&iter = mapped_addrs_.find(addr);
    if (iter == mapped_addrs_.end()) {
      std::cout << "Unmap failed, addr : " << addr << std::endl;
      throw std::runtime_error("unmap failed.");
      return false;
    }
    std::cout << "Unmap addr : " << addr << " - " << std::to_string(reinterpret_cast<size_t>(addr)) << std::endl;
    iter->second->Unmap(addr);
    mapped_addrs_.erase(iter);
    return true;
  }

  std::map<void *, VirtualAddress *> virtual_addresses_;
  std::set<MemHandle *> handles_;

  size_t max_mem_size_;
  size_t base_addr_{0};

  std::map<void *, MemHandle *, Comparator> mapped_addrs_;
};

using aclrtDrvMemHandle = MemHandle *;

class AscendVmmAdapter {
 public:
  static AscendVmmAdapter &GetInstance() {
    static AscendVmmAdapter instance{};
    return instance;
  }

  AscendVmmAdapter() {
    kVmmAlignSize = kDefaultAlignSize;
    std::cout << "VMM align size is " << kVmmAlignSize << std::endl;
  }
  ~AscendVmmAdapter();

 public:
  size_t GetRoundUpAlignSize(size_t input_size) const;
  size_t GetRoundDownAlignSize(size_t input_size) const;

  void ClearAllMemory();
  size_t AllocDeviceMem(size_t size, DeviceMemPtr *addr);
  size_t MmapDeviceMem(const size_t size, const DeviceMemPtr addr, const size_t max_size);
  size_t EagerFreeDeviceMem(const DeviceMemPtr addr, const size_t size);
  size_t GetAllocatedSize() { return physical_handle_size_ * kVmmAlignSize; }

  static const bool IsEnabled() {
    static bool is_enable_vmm = IsVmmEnabled();
    return is_enable_vmm;
  }

 private:
  static const bool IsVmmEnabled() {
    auto enable_vmm = getenv("ENABLE_VMM");
    return enable_vmm != nullptr;
  }

 private:
  uint64_t kVmmAlignSize;
  DeviceMemPtr FindVmmSegment(const DeviceMemPtr addr);
  size_t GetHandleSize(size_t input_size);
  std::atomic<size_t> physical_handle_size_{0};
  std::map<DeviceMemPtr, aclrtDrvMemHandle> vmm_map_;
  std::vector<DeviceMemPtr> all_reserve_mems_;
  std::set<aclrtDrvMemHandle> cached_handle_sets_;
  static constexpr uint64_t kMB = 1024 * 1024;
  static constexpr uint64_t kDefaultAlignSize = 2 * kMB;

  //   static int StringToMB(const std::string &str) {
  //     std::stringstream ss(str);
  //     int num;
  //     std::string unit;
  //     if (!(ss >> num)) {
  //       MS_LOG(EXCEPTION) << "No valid number could be extracted from the string, " << str;
  //     }
  //     if (!(ss >> unit) || unit != "MB") {
  //       MS_LOG(EXCEPTION) << "The unit of the string is not MB, " << str;
  //     }
  //     if (ss.rdbuf()->in_avail() > 0) {
  //       MS_LOG(EXCEPTION) << "The string has extra characters, " << str;
  //     }
  //     return num;
  //   }
  //   static bool CheckVmmDriverVersion() {
  //     // Get driver version
  //     constexpr auto ascend_install_info = "/etc/ascend_install.info";
  //     const std::string DRIVER_INSTALL_PATH_PARAM = "Driver_Install_Path_Param=";
  //     std::string driver_path = "/usr/local/Ascend";

  //     std::ifstream ascend_install_file(ascend_install_info);
  //     if (!ascend_install_file.is_open()) {
  //       MS_LOG(WARNING) << "Open file " << ascend_install_info << " failed.";
  //     } else {
  //       std::string line;
  //       while (std::getline(ascend_install_file, line)) {
  //         size_t pos = line.find(DRIVER_INSTALL_PATH_PARAM);
  //         if (pos != std::string::npos) {
  //           // Extract the path after "Driver_Install_Path_Param="
  //           driver_path = line.substr(pos + DRIVER_INSTALL_PATH_PARAM.length());
  //           MS_LOG(INFO) << "Driver path is " << driver_path;
  //           break;
  //         }
  //       }
  //     }

  //     auto splitString = [](const std::string &str, char delimiter) -> std::vector<std::string> {
  //       std::vector<std::string> tokens;
  //       std::string token;
  //       std::istringstream tokenStream(str);
  //       while (std::getline(tokenStream, token, delimiter)) {
  //         tokens.push_back(token);
  //       }
  //       return tokens;
  //     };

  //     auto driver_version_info = driver_path + "/driver/version.info";
  //     const std::string DRIVER_VERSION_PARAM = "Version=";
  //     std::ifstream driver_version_file(driver_version_info);
  //     if (!driver_version_file.is_open()) {
  //       MS_LOG(WARNING) << "Open file " << driver_version_info << " failed.";
  //     } else {
  //       std::string line;
  //       while (std::getline(driver_version_file, line)) {
  //         size_t pos = line.find(DRIVER_VERSION_PARAM);
  //         if (pos != std::string::npos) {
  //           // Extract the version after "Version="
  //           std::string driver_version = line.substr(pos + DRIVER_VERSION_PARAM.length());
  //           auto split_version = splitString(driver_version, '.');
  //           MS_LOG(INFO) << "Driver version is " << driver_version << ", major version is " << split_version[0];
  //           if (split_version[0] < "24") {
  //             MS_LOG(WARNING) << "Driver version is less than 24.0.0, vmm is disabled by default, drvier_version: "
  //                             << driver_version;
  //             return false;
  //           }
  //           break;
  //         }
  //       }
  //     }
  //     return true;
  //   }
};