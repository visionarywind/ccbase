#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义一个示例结构体
typedef struct {
    int id;
    char name[20];
    float value;
} MyStruct;

// 分配内存并返回一个"隐藏"了原始指针的地址
void* allocate_struct(int id, const char* name, float value) {
    // 分配比结构体大一些的内存，额外空间用于存储原始指针
    size_t total_size = sizeof(MyStruct) + sizeof(MyStruct*);
    char* memory = (char*)malloc(total_size);
    
    if (memory == NULL) {
        return NULL;
    }
    
    // 计算原始结构体应该存放的位置（跳过指针存储区域）
    MyStruct* original_ptr = (MyStruct*)(memory + sizeof(MyStruct*));
    
    // 存储原始指针到额外分配的空间中
    *(MyStruct**)memory = original_ptr;
    
    // 初始化结构体数据
    original_ptr->id = id;
    strncpy(original_ptr->name, name, sizeof(original_ptr->name) - 1);
    original_ptr->value = value;
    
    // 返回的是结构体开始的地址，而非整个内存块的起始地址
    return original_ptr;
}

// 从隐藏的地址中获取原始结构体指针
MyStruct* get_original_ptr(void* hidden_addr) {
    if (hidden_addr == NULL) {
        return NULL;
    }
    
    // 通过地址偏移计算出存储原始指针的位置
    char* memory_start = (char*)hidden_addr - sizeof(MyStruct*);
    return *(MyStruct**)memory_start;
}

// 释放内存
void free_struct(void* hidden_addr) {
    if (hidden_addr == NULL) {
        return;
    }
    
    // 计算整个内存块的起始地址并释放
    char* memory_start = (char*)hidden_addr - sizeof(MyStruct*);
    free(memory_start);
}

int main() {
    // 分配结构体并获取隐藏指针
    void* hidden_ptr = allocate_struct(1, "Test", 3.14f);
    
    if (hidden_ptr == NULL) {
        printf("内存分配失败\n");
        return 1;
    }
    
    // 从隐藏指针中获取原始结构体指针
    MyStruct* original = get_original_ptr(hidden_ptr);
    
    // 使用结构体
    printf("ID: %d\n", original->id);
    printf("Name: %s\n", original->name);
    printf("Value: %.2f\n", original->value);
    
    // 验证我们可以直接通过hidden_ptr访问结构体成员
    printf("\n通过隐藏指针直接访问:\n");
    printf("ID: %d\n", ((MyStruct*)hidden_ptr)->id);
    printf("Name: %s\n", ((MyStruct*)hidden_ptr)->name);
    
    // 释放内存
    free_struct(hidden_ptr);
    
    return 0;
}
