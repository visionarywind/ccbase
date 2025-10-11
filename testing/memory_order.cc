/**
 * 设计实现一个内存序异常的用例挺难的
 * 为了加速过程，可以通过使用ThreadSanitizer加速检测数据竞争，即增加编译参数 -fsanitize=thread
 * 当前依然不复现
*/
#include <atomic>
#include <iostream>
#include <thread>
#include <future>

struct ErrorUseOfMemoryOrder {
  std::atomic<bool> x{0};
  std::atomic<bool> y{0};
  std::atomic<int> z{0};

  void test() {
    std::future<int> consumer_future = std::async([&]() {
      while (!y.load(std::memory_order_relaxed)) {
        std::this_thread::yield();
      }
      if (x.load(std::memory_order_relaxed)) {
        ++z;
      }
      return z.load(std::memory_order_relaxed);
    });
    std::future<void> producer_future = std::async([&]() {
      x.store(true, std::memory_order_relaxed);
      y.store(true, std::memory_order_relaxed);
    });

    auto ret = consumer_future.get();
    if (ret != 1) {
      std::cout << "ERROR : " << ret << std::endl;
    }

    producer_future.get();
  }
};

int main() {
  auto test_count = 100000;
  while (test_count--) {
    ErrorUseOfMemoryOrder order;
    order.test();
  }

  return 0;
}