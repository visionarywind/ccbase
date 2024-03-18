/**
 * Copyright 2023 Huawei Technologies Co., Ltd
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

#ifndef MINDSPORE_CORE_UTILS_FORK_UTILS_H
#define MINDSPORE_CORE_UTILS_FORK_UTILS_H
#include <cstdio>
#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <memory>
#include "utils/ms_utils.h"

#ifdef FORK_UTILS_DEBUG
#define FORK_UTILS_LOG(content, args...) \
  { std::printf("[FORK_UTILS] %s|%d: " #content "\r\n", __func__, __LINE__, ##args); }
#else
#define FORK_UTILS_LOG(content, ...)
#endif

namespace mindspore {
struct fork_callback_info {
  void *class_obj;
  std::function<void()> before_fork_func;
  std::function<void()> parent_atfork_func;
  std::function<void()> child_atfork_func;
};

MS_CORE_API void EmptyFunction();

class MS_CORE_API ForkUtils {
 public:
  static ForkUtils &GetInstance() noexcept;

  template <class T>
  void RegisterCallbacks(std::shared_ptr<T> obj, void (T::*before_fork)(), void (T::*parent_atfork)(),
                         void (T::*child_atfork)()) {
    RegisterCallbacks(obj.get(), before_fork, parent_atfork, child_atfork);
  }

  template <class T>
  void RegisterCallbacks(T *obj, void (T::*before_fork)(), void (T::*parent_atfork)(), void (T::*child_atfork)()) {
#if !defined(_WIN32) && !defined(BUILD_LITE)
    FORK_UTILS_LOG("Register fork callback info.");

    struct fork_callback_info callback_info = {obj, EmptyFunction, EmptyFunction, EmptyFunction};
    if (before_fork) {
      callback_info.before_fork_func = std::bind(before_fork, obj);
    }
    if (parent_atfork) {
      callback_info.parent_atfork_func = std::bind(parent_atfork, obj);
    }
    if (child_atfork) {
      callback_info.child_atfork_func = std::bind(child_atfork, obj);
    }

    bool exist_ = false;
    for (auto &iter : fork_callbacks_) {
      FORK_UTILS_LOG("Callback_info already exist, update info.");
      if (iter.class_obj == obj) {
        exist_ = true;
        iter = callback_info;
        break;
      }
    }
    if (exist_ == false) {
      FORK_UTILS_LOG("Create new callback info.");
      fork_callbacks_.push_back(callback_info);
    }
#endif
  }

  // Note: Do not call this deregistration interface in the destructor of a global object or a singleton object,
  // because fork_utils object may be destructed before them.
  template <class T>
  void DeregCallbacks(const T *obj) noexcept {
#if !defined(_WIN32) && !defined(BUILD_LITE)
    try {
      FORK_UTILS_LOG("Deregister fork callback info.");
      for (auto iter = fork_callbacks_.begin(); iter != fork_callbacks_.end(); (void)++iter) {
        if (iter->class_obj == obj) {
          (void)fork_callbacks_.erase(iter);
          break;
        }
      }
    } catch (const std::exception &e) {
      FORK_UTILS_LOG("Deregister fork callback info failed: %s", e.what());
    } catch (...) {
      FORK_UTILS_LOG("Deregister fork callback info failed: Unknown exception.");
    }
#endif
  }

  std::vector<fork_callback_info> GetCallbacks() { return fork_callbacks_; }

  void SetGilHoldBeforeFork(bool gil_hold_before_fork) { hold_gil_before_fork_ = gil_hold_before_fork; }

  bool IsGilHoldBeforeFork() const { return hold_gil_before_fork_; }

  void SetGilState(int gil_state) { gil_state_ = gil_state; }

  int GetGilState() const { return gil_state_; }

  void BeforeFork();

  void ParentAtFork();

  void ChildAtFork();

 private:
  ForkUtils() = default;
  ~ForkUtils() = default;
  std::vector<fork_callback_info> fork_callbacks_;
  // Record whether forked thread holds the gil lock when the fork occurs.
  bool hold_gil_before_fork_ = false;
  int gil_state_ = 0;
};
}  // namespace mindspore
#endif  // MINDSPORE_CORE_UTILS_FORK_UTILS_H
