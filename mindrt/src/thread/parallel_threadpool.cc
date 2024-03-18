/**
 * Copyright 2022 Huawei Technologies Co., Ltd
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
#ifndef _MSC_VER
#include <sched.h>
#include <unistd.h>
#endif
#include "thread/parallel_threadpool.h"
#include "thread/core_affinity.h"
#include "thread/parallel_thread_pool_manager.h"

namespace mindspore {
constexpr int kActorParallelThreshold = 5;

ParallelWorker::~ParallelWorker() {
  {
    std::lock_guard<std::mutex> _l(mutex_);
    alive_ = false;
  }
  if (enable_shared_thread_pool_) {
    ActivateByOtherPoolTask(nullptr);
  } else {
    cond_var_->notify_one();
  }
  if (thread_->joinable()) {
    thread_->join();
  }
  pool_ = nullptr;
  parallel_pool_ = nullptr;
}

void ParallelWorker::CreateThread() { thread_ = std::make_unique<std::thread>(&ParallelWorker::ParallelRun, this); }

void ParallelWorker::ParallelRun() {
  if (!core_list_.empty()) {
    SetAffinity();
  }
#if !defined(__APPLE__) && !defined(_MSC_VER)
  (void)pthread_setname_np(pthread_self(), ("ParallelThread_" + std::to_string(worker_id_)).c_str());
#endif
#ifdef PLATFORM_86
  // Some CPU kernels need set the flush zero mode to improve performance.
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
#endif
  std::string bind_runner_id = parallel_pool_->GetPoolBindRunnerID();
  enable_shared_thread_pool_ = ParallelThreadPoolManager::GetInstance()->GetEnableSharedThreadPool(bind_runner_id);
  while (alive_) {
    // only run either local KernelTask or PoolQueue ActorTask
    if (RunLocalKernelTask() || RunQueueActorTask()) {
      spin_count_ = 0;
    } else {
      (void)DirectRunOtherPoolTask();
      if (++spin_count_ > max_spin_count_) {
        if (!enable_shared_thread_pool_) {
          WaitUntilActive();
          spin_count_ = 0;
        } else {
          WaitOtherPoolTask();
        }
      } else {
        std::this_thread::yield();
      }
    }
  }
}

void ParallelWorker::WaitUntilActive() {
  std::unique_lock<std::mutex> _l(mutex_);
  cond_var_->wait(_l, [&] { return active_num_ > 0 || !alive_; });
  if (active_num_ > 0) {
    active_num_--;
  }
}

void ParallelWorker::RunOtherPoolTask(ParallelTask *p_task) {
  bool find = false;
  int finish = 0;
  Distributor expected_index = p_task->distributor;
  bool is_busy = false;
  while (p_task->valid && expected_index.started < expected_index.task_num) {
    if (parallel_pool_->GetPoolRef() != 1) {
      is_busy = true;
      break;
    }
    if (p_task->distributor.compare_exchange_strong(expected_index,
                                                    {expected_index.started + 1, expected_index.task_num})) {
      p_task->status |= p_task->func(other_task_->content, expected_index.started, 0, 0);
      find = true;
      expected_index = p_task->distributor;
      finish++;
    }
  }
  if (find && !is_busy) {
    p_task->valid = false;
  }
  p_task->finished += finish;
  return;
}

void ParallelWorker::ActivateByOtherPoolTask(ParallelTask *task) {
  std::unique_lock<std::mutex> l(other_task_mutex_);
  wait_do_other_task_ = false;
  other_task_ = task;
  cv_other_task_.notify_one();
}

void ParallelWorker::WaitOtherPoolTask() {
  std::unique_lock<std::mutex> l(other_task_mutex_);
  while (alive_ && wait_do_other_task_) {
    cv_other_task_.wait(l);
  }
  wait_do_other_task_ = true;
  if (other_task_ == nullptr) {
    return;
  }
  RunOtherPoolTask(other_task_);
  other_task_ = nullptr;
  return;
}

void ParallelWorker::DirectRunOtherPoolTask() {
  std::unique_lock<std::mutex> l(other_task_mutex_);
  if (other_task_ == nullptr) {
    return;
  }
  RunOtherPoolTask(other_task_);
  other_task_ = nullptr;
  return;
}

bool ParallelWorker::RunLocalKernelTask() { return parallel_pool_->RunParallel(); }

bool ParallelWorker::RunQueueActorTask() {
  if (worker_id_ < parallel_pool_->tasks_size()) {
    auto actor = parallel_pool_->PopActorFromQueue();
    if (actor == nullptr) {
      return false;
    }
    actor->Run();
    return true;
  }
  return false;
}

void ParallelThreadPool::UseThreadPool(int num) {
  std::lock_guard<std::mutex> l(mutex_pool_ref_count_);
  pool_ref_count_ += num;
}

bool ParallelThreadPool::SetRunnerID(const std::string &runner_id) {
  if (!bind_runner_id_.empty() &&
      ParallelThreadPoolManager::GetInstance()->GetEnableSharedThreadPool(runner_id) != enable_shared_) {
    THREAD_ERROR("can not set runner id.");
    return false;
  }
  bind_runner_id_ = runner_id;
  return true;
}

std::vector<ParallelWorker *> ParallelThreadPool::GetParallelPoolWorkers() {
  std::vector<ParallelWorker *> workers;
  for (auto woker : workers_) {
    workers.push_back(static_cast<ParallelWorker *>(woker));
  }
  return workers;
}

int ParallelThreadPool::GetPoolRef() {
  std::lock_guard<std::mutex> l(mutex_pool_ref_count_);
  return pool_ref_count_;
}

inline bool ParallelThreadPool::RunTaskOnce(int start, int end) {
  bool find = false;
  ParallelTask *p_task;
  for (int i = start; i < end; i++) {
    if (tasks_[i].valid) {
      if (i != start) {
        tasks_start_ = i;
      }
      int finish = 0;
      p_task = &tasks_[i];
      Distributor expected_index = p_task->distributor;
      while (expected_index.started < expected_index.task_num) {
        if (p_task->distributor.compare_exchange_strong(expected_index,
                                                        {expected_index.started + 1, expected_index.task_num})) {
          p_task->status |= p_task->func(p_task->content, expected_index.started, 0, 0);
          find = true;
          expected_index = p_task->distributor;
          finish++;
        }
      }
      if (find) {
        p_task->valid = false;
        p_task->finished += finish;
        break;
      }
    }
  }
  return find;
}

bool ParallelThreadPool::RunParallel() {
  bool ret = false;
  bool find;
  int max_num = static_cast<int>(tasks_size_);
  if (max_num < kActorParallelThreshold) {
    ParallelTask *p_task;
    do {
      find = false;
      for (int i = 0; i < max_num; i++) {
        if (tasks_[i].valid) {
          int finish = 0;
          p_task = &tasks_[i];
          Distributor expected_index = p_task->distributor;
          while (expected_index.started < expected_index.task_num) {
            if (p_task->distributor.compare_exchange_strong(expected_index,
                                                            {expected_index.started + 1, expected_index.task_num})) {
              p_task->status |= p_task->func(p_task->content, expected_index.started, 0, 0);
              find = true;
              expected_index = p_task->distributor;
              finish++;
            }
          }
          if (find) {
            p_task->valid = false;
            p_task->finished += finish;
            break;
          }
        }
      }
      ret = ret || find;
    } while (find);
  } else {
    do {
      int start = tasks_start_;
      int end = tasks_end_;
      find = false;
      if (start < end) {
        find = RunTaskOnce(start, end);
      } else if (start != end) {
        find = RunTaskOnce(start, max_num);
        if (find == false) {
          find = RunTaskOnce(0, end);
        }
      }
      ret = ret || find;
    } while (find);
  }
  return ret;
}

int ParallelThreadPool::ParallelLaunch(const Func &func, Content content, int task_num) {
  // if single thread, run master thread
  if (task_num <= 1) {
    return SyncRunFunc(func, content, 0, task_num);
  }
  UseThreadPool(1);
  // distribute task to the KernelThread and the idle ActorThread,
  // if the task num is greater than the KernelThread num
  size_t task_index;
  bool expected;
  size_t max_task_num = tasks_size_;

  for (task_index = static_cast<size_t>(tasks_end_); task_index < max_task_num; task_index++) {
    expected = false;
    if (tasks_[task_index].occupied.compare_exchange_strong(expected, true)) {
      tasks_end_ = static_cast<int>(task_index + 1);
      break;
    }
  }
  if (task_index >= max_task_num) {
    for (task_index = 0; task_index < max_task_num; task_index++) {
      expected = false;
      if (tasks_[task_index].occupied.compare_exchange_strong(expected, true)) {
        tasks_end_ = static_cast<int>(task_index + 1);
        break;
      }
    }
    if (task_index >= max_task_num) {
      return SyncRunFunc(func, content, 0, task_num);
    }
  }

  ParallelTask *p_task = &tasks_[task_index];
  p_task->valid.store(false);
  p_task->func = func;
  p_task->content = content;
  p_task->finished = 1;
  p_task->distributor = {1, task_num};
  p_task->valid.store(true);

  ParallelThreadPool *idle_pool = nullptr;
  if (!enable_shared_) {
    ActiveWorkers();
  } else {
    for (auto &worker : workers_) {
      static_cast<ParallelWorker *>(worker)->ActivateByOtherPoolTask();
    }
    if (thread_num_ < task_num) {
      idle_pool = ParallelThreadPoolManager::GetInstance()->GetIdleThreadPool(bind_runner_id_, p_task);
    }
  }

  p_task->status |= p_task->func(p_task->content, 0, 0, 0);

  Distributor expected_index = p_task->distributor;
  while (expected_index.started < task_num) {
    if (p_task->distributor.compare_exchange_strong(expected_index, {expected_index.started + 1, task_num})) {
      p_task->status |= p_task->func(p_task->content, expected_index.started, 0, 0);
      (void)++p_task->finished;
      expected_index = p_task->distributor;
    }
  }
  p_task->valid = false;
  while (p_task->finished < task_num) {
    std::this_thread::yield();
  }
  p_task->occupied = false;
  // check the return value of task
  if (p_task->status != THREAD_OK) {
    return THREAD_ERROR;
  }
  if (idle_pool != nullptr) {
    idle_pool->UseThreadPool(-1);
  }
  UseThreadPool(-1);
  return THREAD_OK;
}

bool ParallelThreadPool::IsIdlePool() {
  auto export_ref_count = 0;
  if (this->pool_ref_count_.compare_exchange_strong(export_ref_count, 1)) {
    return true;
  }
  return false;
}

int ParallelThreadPool::CreateParallelThreads(size_t actor_thread_num, size_t all_thread_num,
                                              const std::vector<int> &core_list) {
  if (actor_thread_num == 0) {
    THREAD_ERROR("thread num is invalid");
    return THREAD_ERROR;
  }
  if (ActorQueueInit() != THREAD_OK) {
    return THREAD_ERROR;
  }
  if (affinity_ != nullptr) {
    affinity_->SetCoreId(core_list);
  }
  size_t core_num = std::thread::hardware_concurrency();
  all_thread_num = all_thread_num < core_num ? all_thread_num : core_num;
  actor_thread_num_ = actor_thread_num < all_thread_num ? actor_thread_num : all_thread_num;
  size_t tasks_size = actor_thread_num;

  tasks_ = new (std::nothrow) ParallelTask[tasks_size]();
  THREAD_ERROR_IF_NULL(tasks_);
  tasks_size_ = tasks_size;
  if (TaskQueuesInit(all_thread_num) != THREAD_OK) {
    return THREAD_ERROR;
  }
  enable_shared_ = ParallelThreadPoolManager::GetInstance()->GetEnableSharedThreadPool(bind_runner_id_);
  auto ret = ThreadPool::CreateThreads<ParallelWorker>(all_thread_num, core_list);
  if (ret != THREAD_OK) {
    return THREAD_ERROR;
  }
  thread_num_ = static_cast<int>(thread_num());
  return THREAD_OK;
}

ParallelThreadPool *ParallelThreadPool::CreateThreadPool(size_t actor_thread_num, size_t all_thread_num,
                                                         const std::vector<int> &core_list, BindMode bind_mode,
                                                         std::string runner_id) {
  std::lock_guard<std::mutex> lock(create_thread_pool_muntex_);
  ParallelThreadPool *pool = new (std::nothrow) ParallelThreadPool();
  if (pool == nullptr) {
    return nullptr;
  }
  if (!pool->SetRunnerID(runner_id)) {
    delete pool;
    return nullptr;
  }
  int ret = pool->InitAffinityInfo();
  if (ret != THREAD_OK) {
    delete pool;
    return nullptr;
  }
  if (core_list.empty()) {
    ret = pool->CreateParallelThreads(actor_thread_num, all_thread_num,
                                      pool->affinity_->GetCoreId(all_thread_num, bind_mode));
  } else {
    ret = pool->CreateParallelThreads(actor_thread_num, all_thread_num, core_list);
  }
  if (ret != THREAD_OK) {
    delete pool;
    return nullptr;
  }
  return pool;
}
}  // namespace mindspore
