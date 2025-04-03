#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>

std::unordered_map<std::shared_ptr<int>, int> myMap;
std::mutex mutex_;

// 写操作线程函数
void writer() {
    // for (int i = 0; i < 1000; ++i) {
    int i = 0;
    while (true) {
      i++;
      std::lock_guard<std::mutex> lock(mutex_);
      std::shared_ptr<int> key = std::make_shared<int>(i);
      myMap[key] = i * 2;
      while (i > 100000) {
        auto iter = myMap.begin();
        myMap.erase(iter);
        break;
      }
    }
}

// 读操作线程函数
void reader() {
  while (true) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = myMap.begin();
    while (it != myMap.end()) {
        std::cout << "Key: " << *(it->first) << ", Value: " << it->second << std::endl;
        break;
    }
  }
}

int main() {
    std::thread writerThread1(writer);
    std::thread writerThread2(writer);
    std::thread readerThread1(reader);
    std::thread readerThread2(reader);

    writerThread1.join();
    writerThread2.join();
    readerThread1.join();
    readerThread2.join();

    return 0;
}