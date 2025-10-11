#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>
#include <cassert>

// 共享数据
struct SharedData {
    std::atomic<int> data{0};
    std::atomic<bool> ready{false};
};

// 错误的生产者实现：使用过于松散的内存序
void producer_error(SharedData& shared) {
    int important_value = 42;
    
    // 存储数据 - 使用过于松散的内存序
    shared.data.store(important_value, std::memory_order_relaxed);  // #1
    
    // 存储就绪标志 - 同样使用松散内存序
    shared.ready.store(true, std::memory_order_relaxed);           // #2
    
    // std::cout << "Producer: data = " << important_value << ", ready = true" << std::endl;
}

// 错误的消费者实现
void consumer_error(SharedData& shared) {
    // 等待就绪标志 - 使用松散内存序
    while (!shared.ready.load(std::memory_order_relaxed)) {        // #3
        std::this_thread::yield();
    }
    
    // 读取数据 - 使用松散内存序
    int value = shared.data.load(std::memory_order_relaxed);       // #4
    
    // std::cout << "Consumer: received data = " << value << std::endl;
    
    // 这里可能会出现断言失败！
    // 因为消费者可能看到 ready=true 但 data 仍然是旧值(0)
    if (value == 0) {
        std::cout << "ERROR: Consumer saw ready flag but data is still 0!" << std::endl;
    }
}

// 演示问题的测试函数
void test_memory_order_error() {
    std::cout << "=== Testing Memory Order Error ===" << std::endl;
    
    const int test_runs = 1000000;
    int error_count = 0;
    
    for (int i = 0; i < test_runs; ++i) {
        SharedData shared;
        
        std::thread producer(producer_error, std::ref(shared));
        std::thread consumer(consumer_error, std::ref(shared));
        
        producer.join();
        consumer.join();
        
        // 检查是否出现了数据竞争导致的问题
        int final_data = shared.data.load(std::memory_order_relaxed);
        if (final_data == 0) {
            ++error_count;
        }
    }
    
    std::cout << "Test completed: " << error_count << " errors out of " << test_runs << " runs" << std::endl;
    std::cout << "Error rate: " << (100.0 * error_count / test_runs) << "%" << std::endl;
}

// 更复杂的错误示例：多个数据字段的同步问题
struct MultiData {
    std::atomic<int> x{0};
    std::atomic<int> y{0};
    std::atomic<bool> initialized{false};
};

void writer_error(MultiData& data) {
    // 初始化两个相关字段
    data.x.store(100, std::memory_order_relaxed);      // #A
    data.y.store(200, std::memory_order_relaxed);      // #B
    
    // 设置初始化完成标志
    data.initialized.store(true, std::memory_order_relaxed);  // #C
}

void reader_error(MultiData& data) {
    while (!data.initialized.load(std::memory_order_relaxed)) {  // #D
        std::this_thread::yield();
    }
    
    int x_val = data.x.load(std::memory_order_relaxed);          // #E
    int y_val = data.y.load(std::memory_order_relaxed);          // #F
    
    // 可能出现 x=100, y=0 或者 x=0, y=200 的情况！
    if ((x_val == 100 && y_val == 0) || (x_val == 0 && y_val == 200)) {
        std::cout << "INCONSISTENT STATE: x=" << x_val << ", y=" << y_val << std::endl;
    }
    
    // 期望的关系可能不成立
    if (x_val != 0 && y_val != 0 && y_val != 2 * x_val) {
        std::cout << "RELATIONSHIP BROKEN: y should be 2*x, but x=" 
                  << x_val << ", y=" << y_val << std::endl;
    }
}

void test_multi_data_error() {
    std::cout << "\n=== Testing Multi-Data Memory Order Error ===" << std::endl;
    
    const int test_runs = 10000;
    int inconsistent_count = 0;
    int relationship_broken_count = 0;
    
    for (int i = 0; i < test_runs; ++i) {
        MultiData data;
        
        std::thread writer(writer_error, std::ref(data));
        std::thread reader(reader_error, std::ref(data));
        
        writer.join();
        reader.join();
    }
}

// 正确的实现：使用 acquire-release 语义
void producer_correct(SharedData& shared) {
    int important_value = 42;
    
    // 先存储数据
    shared.data.store(important_value, std::memory_order_relaxed);
    
    // 使用 release 语义存储就绪标志
    // 保证之前的所有内存操作（包括data的存储）在ready=true对消费者可见之前完成
    shared.ready.store(true, std::memory_order_release);  // 正确的同步点
    
    std::cout << "Correct producer finished" << std::endl;
}

void consumer_correct(SharedData& shared) {
    // 使用 acquire 语义加载就绪标志
    // 保证看到ready=true时，也能看到producer中release之前的所有内存操作
    while (!shared.ready.load(std::memory_order_acquire)) {  // 与producer的release配对
        std::this_thread::yield();
    }
    
    // 现在可以安全地读取data，使用relaxed即可
    int value = shared.data.load(std::memory_order_relaxed);
    
    std::cout << "Correct consumer: received data = " << value << std::endl;
    
    // 这个断言永远不会失败
    assert(value == 42);
}

void test_correct_implementation() {
    std::cout << "\n=== Testing Correct Implementation ===" << std::endl;
    
    SharedData shared;
    
    std::thread producer(producer_correct, std::ref(shared));
    std::thread consumer(consumer_correct, std::ref(shared));
    
    producer.join();
    consumer.join();
    
    std::cout << "Correct implementation test passed!" << std::endl;
}

int main() {
    // 测试错误的内存序使用
    test_memory_order_error();
    
    // 测试多数据字段的同步问题
    test_multi_data_error();
    
    // 测试正确的实现
    test_correct_implementation();
    
    return 0;
}