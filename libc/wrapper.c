#include <iostream>
#include <chrono>

// 定义被测试的函数
void function_to_test() {
    // 实际被测试的函数逻辑，可以是复杂的计算等
    for (int i = 0; i < 1000000; ++i) {
        // 简单的计算示例
        int result = i * i;
        (void)result;
    }
}

// 包装函数
void __wrap_function_to_test() {
    auto start = std::chrono::high_resolution_clock::now();
    // 调用原始函数
    __real_function_to_test();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "function_to_test took " << duration << " milliseconds." << std::endl;
}