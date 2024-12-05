#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <vector>

struct Tick {
  Tick(size_t cost, const std::thread::id &thread_id) : cost_(cost), thread_id_(thread_id) {}

  std::string ToString() {
    std::stringstream buffer;
    buffer << "cost : " << std::to_string(cost_) << " ns, thread id : " << thread_id_ << "\n";
    return buffer.str();
  }

  size_t cost_;
  std::thread::id thread_id_;
};
using TickPtr = std::shared_ptr<Tick>;

struct Perf {
  Perf() { std::cout << "Perf constructed." << std::endl; }
  ~Perf() { std::cout << "Perf deconstructed." << std::endl; }

  void Dump(const std::string &key) {
    for (const auto &tick : ticks_) {
      std::cout << tick->ToString() << std::endl << std::flush;
    }

    std::sort(ticks_.begin(), ticks_.end());
    // Calculate mean and median, then print them.
    auto total_size = ticks_.size();
    double median = 0;
    if (total_size & 1) {
      median = (ticks_[total_size >> 1]->cost_ + ticks_[(total_size >> 1) + 1]->cost_) >> 1;
    } else {
      median = ticks_[total_size >> 1]->cost_;
    }
    std::cout << "key : " << key << "\n" << std::flush;
    std::cout << "total : " << total_size << "\n" << std::flush;
    std::cout << "median : " << median << "ns.\n" << std::flush;
    double sum = 0;
    for (size_t i = 0; i < total_size; i++) {
      if (ticks_[i]->cost_ <= 1000000 && ticks_[i]->cost_ >= 200) sum += ticks_[i]->cost_;
    }
    double mean = sum / total_size;
    std::cout << "mean : " << mean << "ns.\n" << std::flush;
  }

  void Record() { Record(GetTick()); }

  uint64_t GetTick() {
    auto &&ts = std::chrono::system_clock::now();
    int64_t system_t = std::chrono::duration_cast<std::chrono::nanoseconds>(ts.time_since_epoch()).count();
    return static_cast<uint64_t>(system_t);
  }

  void Record(size_t cost) { (void)ticks_.emplace_back(std::make_shared<Tick>(cost, std::this_thread::get_id())); }

  std::vector<TickPtr> ticks_;
};
using PerfPtr = std::shared_ptr<Perf>;

struct PerfCounter {
  ~PerfCounter() {
    for (const auto &kv : perfs_) {
      std::cout << "Dump : " << kv.first << std::endl << std::flush;
      (void)kv.second->Dump(kv.first);
    }
  }
  void Record(const std::string &key) {
    mutex_.lock();
    auto it = perfs_.find(key);
    if (it == perfs_.end()) {
      perfs_[key] = std::make_shared<Perf>();
    }

    perfs_[key]->Record();
    mutex_.unlock();
  }

  void Record(const std::string &key, size_t cost) {
    mutex_.lock();
    auto it = perfs_.find(key);
    if (it == perfs_.end()) {
      perfs_[key] = std::make_shared<Perf>();
    }

    perfs_[key]->Record(cost);
    mutex_.unlock();
  }

  std::mutex mutex_;
  std::map<std::string, PerfPtr> perfs_;
};

PerfCounter perf_counter;

int main() {
  for (size_t i = 0; i < 1000; i++) perf_counter.Record("");
  return 0;
}