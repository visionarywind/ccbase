#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MEMORY_SIZE 4096

int main() {
  // 步骤 1: 使用 mmap 分配匿名内存
  // MAP_ANONYMOUS 表示创建匿名映射，不与任何文件关联
  // MAP_PRIVATE 表示对映射区域的修改是私有的，不会反映到文件中（这里无文件关联）
  // PROT_READ | PROT_WRITE 表示该内存区域可读可写
  void *mapped_memory = mmap(NULL, MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (mapped_memory == MAP_FAILED) {
    perror("mmap");
    return 1;
  }

  // 步骤 2: 向映射的内存区域写入数据
  const char *message = "Hello, mmap!";
  strcpy((char *)mapped_memory, message);

  // 步骤 3: 从映射的内存区域读取数据并打印
  printf("Read from mapped memory: %s\n", (char *)mapped_memory);

  // 步骤 4: 使用 munmap 释放映射的内存
  if (munmap(mapped_memory, MEMORY_SIZE) == -1) {
    perror("munmap");
    return 1;
  }

  printf("Memory unmapped successfully.\n");

  return 0;
}