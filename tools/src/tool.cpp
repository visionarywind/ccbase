#include <tool.h>

void CountDownLatch::await() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this]() { return count_ <= 0; });
}

bool CountDownLatch::await(int milliseconds) {
    std::unique_lock<std::mutex> lock(mutex_);
    return cv_.wait_for(lock, std::chrono::milliseconds(milliseconds), [this]() { return count_ <= 0; });
}

void CountDownLatch::count_down() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (count_ > 0) {
        count_--;
        cv_.notify_all();
    }
}

int CountDownLatch::count() {
    std::unique_lock<std::mutex> lck(mutex_);
    return count_;
}