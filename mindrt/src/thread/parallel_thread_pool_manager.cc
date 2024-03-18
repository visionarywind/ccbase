/**
 * Copyright 2022-2023 Huawei Technologies Co., Ltd
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

#include "thread/parallel_thread_pool_manager.h"
#include <map>
#include <string>
#include "thread/parallel_threadpool.h"

namespace mindspore {
namespace {
const char *kInnerModelParallelRunner = "inner_model_parallel_runner";
const char *kInnerRunnerID = "inner_runner_id";
const char *kInnerModelID = "inner_model_id";
}  // namespace
ParallelThreadPoolManager *ParallelThreadPoolManager::GetInstance() {
  static ParallelThreadPoolManager instance;
  return &instance;
}

void ParallelThreadPoolManager::Init(bool enable_shared_thread_pool, const std::string &runner_id, int worker_num,
                                     int remaining_thread_num, int thread_num_limit) {
  std::unique_lock<std::shared_mutex> l(pool_manager_mutex_);
  if (enable_shared_thread_pool_.find(runner_id) != enable_shared_thread_pool_.end()) {
    THREAD_ERROR("Not need to repeat init.");
    return;
  }
  enable_shared_thread_pool_[runner_id] = enable_shared_thread_pool;
  if (!enable_shared_thread_pool) {
    THREAD_INFO("not enable shared parallel thread pool.");
    return;
  }
  std::vector<ParallelThreadPool *> runner_pools(worker_num, nullptr);
  runner_id_pools_[runner_id] = runner_pools;
  remaining_thread_num_[runner_id] = remaining_thread_num;
  thread_num_limit_[runner_id] = thread_num_limit;
  idle_pool_num_[runner_id] = worker_num;
  runner_worker_num_[runner_id] = worker_num;
  worker_init_num_[runner_id] = 0;
}

void ParallelThreadPoolManager::SetHasIdlePool(std::string runner_id, bool is_idle) {
  std::unique_lock<std::shared_mutex> l(pool_manager_mutex_);
  has_idle_pool_[runner_id] = is_idle;
}

int ParallelThreadPoolManager::GetTaskNum(
  const std::map<std::string, std::map<std::string, std::string>> *config_info) {
  if (config_info == nullptr) {
    THREAD_ERROR("config_info is nullptr.");
    return -1;
  }
  std::string runner_id;
  auto it_id = config_info->find(kInnerModelParallelRunner);
  if (it_id != config_info->end()) {
    auto item_runner = it_id->second.find(kInnerRunnerID);
    if (item_runner != it_id->second.end()) {
      runner_id = it_id->second.at(kInnerRunnerID);
    }
  }
  std::unique_lock<std::shared_mutex> l(pool_manager_mutex_);
  if (runner_id.empty() || enable_shared_thread_pool_.find(runner_id) == enable_shared_thread_pool_.end() ||
      !enable_shared_thread_pool_[runner_id]) {
    THREAD_INFO("not enable shared parallel thread pool.");
    return -1;
  }
  return thread_num_limit_[runner_id];
}

int ParallelThreadPoolManager::GetThreadPoolSize(ThreadPool *pool) {
  std::unique_lock<std::shared_mutex> l(pool_manager_mutex_);
  ParallelThreadPool *thread_pool = static_cast<ParallelThreadPool *>(pool);
  if (thread_pool == nullptr) {
    return -1;
  }
  if (pool_workers_.find(thread_pool) != pool_workers_.end()) {
    return pool_workers_[thread_pool].size();
  } else {
    return -1;
  }
  return -1;
}

void ParallelThreadPoolManager::BindPoolToRunner(
  ThreadPool *pool, const std::map<std::string, std::map<std::string, std::string>> *config_info) {
  std::unique_lock<std::shared_mutex> l(pool_manager_mutex_);
  if (config_info == nullptr) {
    THREAD_ERROR("config_info is nullptr.");
    return;
  }
  std::string runner_id;
  auto it_id = config_info->find(kInnerModelParallelRunner);
  if (it_id != config_info->end()) {
    auto item_runner = it_id->second.find(kInnerRunnerID);
    if (item_runner != it_id->second.end()) {
      runner_id = it_id->second.at(kInnerRunnerID);
    }
  }
  if (enable_shared_thread_pool_.find(runner_id) == enable_shared_thread_pool_.end() ||
      !enable_shared_thread_pool_[runner_id]) {
    THREAD_ERROR("not use parallel thread pool shared.");
    return;
  }
  auto parallel_pool = static_cast<ParallelThreadPool *>(pool);
  if (parallel_pool == nullptr) {
    THREAD_ERROR("parallel pool is nullptr.");
  }
  int model_id = 0;
  auto item_runner = it_id->second.find(kInnerModelID);
  if (item_runner != it_id->second.end()) {
    model_id = std::atoi(it_id->second.at(kInnerModelID).c_str());
  }
  auto runner_id_pools_iter = runner_id_pools_.find(runner_id);
  if (runner_id_pools_iter == runner_id_pools_.end()) {
    return;
  }
  auto &runner_id_pools = runner_id_pools_iter->second;
  if (static_cast<size_t>(model_id) >= runner_id_pools.size()) {
    return;
  }
  runner_id_pools_[runner_id].at(model_id) = parallel_pool;
  auto all_workers = parallel_pool->GetParallelPoolWorkers();
  for (size_t i = 0; i < all_workers.size(); i++) {
    auto worker = static_cast<ParallelWorker *>(all_workers[i]);
    pool_workers_[parallel_pool].push_back(worker);
  }
  worker_init_num_[runner_id]++;
}

bool ParallelThreadPoolManager::GetEnableSharedThreadPool(std::string runner_id) {
  std::unique_lock<std::shared_mutex> l(pool_manager_mutex_);
  if (enable_shared_thread_pool_.find(runner_id) == enable_shared_thread_pool_.end()) {
    return false;
  }
  return enable_shared_thread_pool_[runner_id];
}

void ParallelThreadPoolManager::ActivatePool(const std::string &runner_id, int model_id) {
  std::shared_lock<std::shared_mutex> l(pool_manager_mutex_);
  if (enable_shared_thread_pool_.find(runner_id) == enable_shared_thread_pool_.end() ||
      !enable_shared_thread_pool_[runner_id]) {
    return;
  }
  if (idle_pool_num_.find(runner_id) == idle_pool_num_.end()) {
    return;
  }
  auto runner_id_pools_iter = runner_id_pools_.find(runner_id);
  if (runner_id_pools_iter == runner_id_pools_.end()) {
    return;
  }
  idle_pool_num_[runner_id]--;
  auto &runner_id_pools = runner_id_pools_iter->second;
  if (static_cast<size_t>(model_id) < runner_id_pools.size()) {
    auto &pool = runner_id_pools_iter->second[model_id];
    pool->UseThreadPool(1);

    auto pool_workers_iter = pool_workers_.find(pool);
    if (pool_workers_iter != pool_workers_.end()) {
      auto &workers = pool_workers_iter->second;
      for (auto &worker : workers) {
        worker->ActivateByOtherPoolTask();
      }
    }
  }
}

void ParallelThreadPoolManager::SetFreePool(const std::string &runner_id, int model_id) {
  std::shared_lock<std::shared_mutex> l(pool_manager_mutex_);
  if (enable_shared_thread_pool_.find(runner_id) == enable_shared_thread_pool_.end() ||
      !enable_shared_thread_pool_[runner_id]) {
    return;
  }
  auto runner_id_pools_iter = runner_id_pools_.find(runner_id);
  if (runner_id_pools_iter == runner_id_pools_.end()) {
    return;
  }
  if (idle_pool_num_.find(runner_id) == idle_pool_num_.end()) {
    return;
  }
  auto &runner_id_pools = runner_id_pools_iter->second;
  if (static_cast<size_t>(model_id) < runner_id_pools.size()) {
    auto &pool = runner_id_pools_iter->second[model_id];
    pool->UseThreadPool(-1);
    idle_pool_num_[runner_id]++;
  }
}

ParallelThreadPool *ParallelThreadPoolManager::GetIdleThreadPool(const std::string &runner_id, ParallelTask *task) {
  std::shared_lock<std::shared_mutex> l(pool_manager_mutex_);
  auto runner_worker_num_iter = runner_worker_num_.find(runner_id);
  auto worker_init_num_iter = worker_init_num_.find(runner_id);
  if (runner_worker_num_iter == runner_worker_num_.end() || worker_init_num_iter == worker_init_num_.end() ||
      runner_worker_num_iter->second != worker_init_num_iter->second) {
    return nullptr;
  }
  auto idle_pool_num_iter = idle_pool_num_.find(runner_id);
  if (idle_pool_num_iter == idle_pool_num_.end() || idle_pool_num_iter->second <= 0) {
    return nullptr;
  }

  auto runner_id_pools_iter = runner_id_pools_.find(runner_id);
  if (runner_id_pools_iter == runner_id_pools_.end()) {
    return nullptr;
  }
  auto &all_pools = runner_id_pools_iter->second;
  for (int pool_index = all_pools.size() - 1; pool_index >= 0; pool_index--) {
    auto &pool = all_pools[pool_index];
    if (pool->IsIdlePool()) {
      auto pool_workers_iter = pool_workers_.find(pool);
      if (pool_workers_iter == pool_workers_.end()) {
        pool->UseThreadPool(-1);
        continue;
      }
      auto &workers = pool_workers_iter->second;
      auto remaining_thread_num_iter = remaining_thread_num_.find(runner_id);
      if (remaining_thread_num_iter == remaining_thread_num_.end()) {
        pool->UseThreadPool(-1);
        continue;
      }
      for (size_t i = 0; i < workers.size() - remaining_thread_num_iter->second; i++) {
        workers[i]->ActivateByOtherPoolTask(task);
      }
      return pool;
    }
  }
  return nullptr;
}

void ParallelThreadPoolManager::ResetParallelThreadPoolManager(const std::string &runner_id) {
  std::unique_lock<std::shared_mutex> l(pool_manager_mutex_);
  if (runner_id_pools_.find(runner_id) == runner_id_pools_.end()) {
    return;
  }
  auto pools = runner_id_pools_[runner_id];
  for (auto &pool : pools) {
    pool_workers_.erase(pool);
  }
  runner_id_pools_.erase(runner_id);
  has_idle_pool_.erase(runner_id);
  enable_shared_thread_pool_.erase(runner_id);
  remaining_thread_num_.erase(runner_id);
  thread_num_limit_.erase(runner_id);
  runner_worker_num_.erase(runner_id);
  worker_init_num_.erase(runner_id);
  idle_pool_num_.erase(runner_id);
}

ParallelThreadPoolManager::~ParallelThreadPoolManager() {
  THREAD_INFO("~ParallelThreadPoolManager start.");
  std::unique_lock<std::shared_mutex> l(pool_manager_mutex_);
  pool_workers_.clear();
  runner_id_pools_.clear();
  has_idle_pool_.clear();
  enable_shared_thread_pool_.clear();
  remaining_thread_num_.clear();
  thread_num_limit_.clear();
  runner_worker_num_.clear();
  worker_init_num_.clear();
  idle_pool_num_.clear();
  THREAD_INFO("~ParallelThreadPoolManager end.");
}
}  // namespace mindspore
