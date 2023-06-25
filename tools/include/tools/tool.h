#pragma once

#include <mutex>

class CountDownLatch {
public:
    explicit CountDownLatch(uint32_t count = 1) : count_(count) {}

    void await();

    bool await(int milliseconds);

    void count_down();

    int count();

private:
    CountDownLatch() = delete;
    CountDownLatch(const CountDownLatch &other) = delete;
    CountDownLatch &operator=(const CountDownLatch &other) = delete;

    std::mutex mutex_;
    std::condition_variable cv_;
    uint32_t count_;
};
