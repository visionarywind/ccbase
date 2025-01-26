#include <iostream>
#include <chrono>
#include <functional>
#include <list>
#include <thread>
#include <condition_variable>

std::mutex kMutex;
std::condition_variable cond_var;
bool kReady = false;
std::list<std::function<void()>> kList;

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(kMutex);
        // 等待条件成立
        cond_var.wait(lock, [] { return kReady; });
        auto func = kList.front();
        kList.pop_front();
        // std::cout << "call func" << std::endl;
        func();
        // std::cout << "end call func" << std::endl;
        kReady = false;
    }
}

void producer() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); 
        std::lock_guard<std::mutex> lock(kMutex);

        std::cout << "Produce" << std::endl;
        std::string s = "gege";
        kList.push_back([s]() {
            std::cout << s << std::endl;
        });

        kReady = true;
        cond_var.notify_one();
    }
}

int main() {
    std::thread consumer_thread(consumer);
    std::thread producer_thread(producer);

    consumer_thread.join();
    producer_thread.join();

    return 1;
}