#include <time.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/stat.h>
// #include <threads.h>

#define FILE_SIZE (1UL << 30)  // 1GB
#define BLOCK_SIZE 4096        // 4KB
#define NUM_THREADS 8

static atomic_size_t g_offset;
static const char *FILE_NAME = "concurrent_file.dat";

typedef struct {
  char *mapped_ptr;
  size_t file_size;
  size_t block_size;
  size_t index;
} ThreadArgs;

#define MAX_STACK_FRAMES 192
typedef struct {
  int pid;
  char ops_type;
  void *ptr;
  size_t size;
  int frames;
  void *buffer[MAX_STACK_FRAMES];
} ExecStack;

ExecStack *GetMappingPtr();

void *thread_write(void *arg) {
  // ThreadArgs* args = (ThreadArgs*)arg;
  // char* map = args->mapped_ptr;
  // const size_t total_size = args->file_size;
  // const size_t block_size = args->block_size;
  int count = 1000;
  while (count--) {
    // 原子获取当前偏移并增加块大小
    // size_t old_offset = atomic_fetch_add_explicit(&g_offset, block_size, memory_order_relaxed);

    // // 检查是否超出文件范围
    // if (old_offset >= total_size) {
    //     break;
    // }

    // // 计算实际写入大小
    // size_t remaining = total_size - old_offset;
    // size_t write_size = (remaining < block_size) ? remaining : block_size;
    ExecStack *ptr = GetMappingPtr();
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    // 写入数据（这里用线程号填充不同ASCII字符作为示例）
    char value = 'A' + (char)(timeout.tv_nsec % 26);
    memset(ptr, 'A' + (char)(timeout.tv_nsec % 26), sizeof(ExecStack));
    printf("write for %c, size : %zu, ptr : %p\n", value, sizeof(ExecStack), ptr);
  }

  return NULL;
}

typedef struct MappingFile {
  int file_;
  atomic_int initialize_;
  size_t mapping_size_;
  atomic_int write_offset_;
  size_t mapping_offset_;
  atomic_int spin_lock_;
  char *mapped_ptr_;
} MappingFile;

MappingFile kMappingFile;

void spinlock_init(atomic_int *lock) {
  printf("%d - spin lock init : %p\n", (int)getpid(), lock);
  atomic_store_explicit(lock, 0, memory_order_relaxed);
}

void spinlock_lock(atomic_int *lock) {
  // malloc_printf("lock : %p\n", lock);
  while (atomic_exchange_explicit(lock, 1, memory_order_acquire)) {
    while (atomic_load_explicit(lock, memory_order_relaxed)) {
      // thrd_yield();
    }
  }
}

void spinlock_unlock(atomic_int *lock) {
  // malloc_printf("unlock : %p\n", lock);
  atomic_store_explicit(lock, 0, memory_order_release);
}

void ExtandMappingFile();

void InitMappingFile() {
  printf("init mapping file\n");
  if (atomic_load_explicit(&kMappingFile.initialize_, memory_order_acquire) == 1) {
    printf("exit init mapping file\n");
    return;
  }
  atomic_store_explicit(&kMappingFile.initialize_, 1, memory_order_release);
  pid_t pid = getpid();
  char file_name[100];
  printf("init mapping file start\n");
  sprintf(file_name, "tracker_%d.dat", (int)pid);
  kMappingFile.file_ = open(file_name, O_RDWR | O_CREAT, 0644);
  if (kMappingFile.file_ == 0) {
    perror("Error opening file");
    return;
  }
  printf("open file : %d\n", kMappingFile.file_);

  kMappingFile.mapped_ptr_ = NULL;
  spinlock_init(&kMappingFile.spin_lock_);
  kMappingFile.mapping_size_ = 1024 * sizeof(ExecStack);
  printf("init mapping file size : %zu\n", kMappingFile.mapping_size_);
  atomic_store_explicit(&kMappingFile.write_offset_, 0, memory_order_release);
  kMappingFile.mapping_offset_ = 0;
  ExtandMappingFile();
}

void ExtandMappingFile() {
  // unmmap old mapping.
  if (kMappingFile.mapped_ptr_ != NULL) {
    printf("try unmmap older file\n");
    if (munmap(kMappingFile.mapped_ptr_, kMappingFile.mapping_size_) == -1) {
      perror("munmap failed");
      close(kMappingFile.file_);
      return;
    }
  }

  // make sure file is large enough.
  size_t file_expected_size = kMappingFile.mapping_size_ + kMappingFile.mapping_offset_;
  printf("try to extand file : %d, size : %zu, mapping size : %zu\n", kMappingFile.file_, file_expected_size,
         kMappingFile.mapping_size_);
  if (ftruncate(kMappingFile.file_, file_expected_size) == -1) {
    perror("ftruncate failed");
    close(kMappingFile.file_);
    return;
  }

  // do mmapping.
  printf("try to mapping file : %d\n", kMappingFile.file_);
  kMappingFile.mapped_ptr_ = mmap(NULL,                        // 内核自动选择映射地址
                                  kMappingFile.mapping_size_,  // 映射区域大小（与文件调整后的大小匹配）
                                  PROT_READ | PROT_WRITE,      // 映射区域可读可写
                                  MAP_SHARED,                  // 修改会同步到文件（关键！否则写操作不会保存）
                                  kMappingFile.file_,          // 文件描述符
                                  0                            // 从文件起始位置开始映射
  );
  if (kMappingFile.mapped_ptr_ == MAP_FAILED) {
    perror("mmap failed");
    close(kMappingFile.file_);
    return;
  }
  // update offset
  kMappingFile.mapping_offset_ += kMappingFile.mapping_size_;
}

ExecStack *GetMappingPtr() {
  printf("get mapping ptr\n");
  // if (atomic_load_explicit(&kMappingFile.initialize_, memory_order_acquire) == 0) {
  //   InitMappingFile();
  // }

  spinlock_lock(&kMappingFile.spin_lock_);

  // check if need extand mapping file.
  if (atomic_load_explicit(&kMappingFile.write_offset_, memory_order_relaxed) + sizeof(ExecStack) >=
      kMappingFile.mapping_size_) {
    ExtandMappingFile();
  }

  char *ptr = kMappingFile.mapped_ptr_ + atomic_load_explicit(&kMappingFile.write_offset_, memory_order_relaxed);
  atomic_fetch_add_explicit(&kMappingFile.write_offset_, sizeof(ExecStack), memory_order_relaxed);
  spinlock_unlock(&kMappingFile.spin_lock_);
  return (ExecStack *)ptr;
}

void CleanMappingFile() {
  if (kMappingFile.mapped_ptr_ != NULL) {
    if (munmap(kMappingFile.mapped_ptr_, kMappingFile.mapping_size_) == -1) {
      perror("munmap failed");
      close(kMappingFile.file_);
      return;
    }
  }

  size_t file_size = kMappingFile.write_offset_ + sizeof(ExecStack);
  printf("truncate file size : %zu\n", file_size);
  if (ftruncate(kMappingFile.file_, file_size) == -1) {
    perror("ftruncate failed");
    close(kMappingFile.file_);
    return;
  }
  close(kMappingFile.file_);
}

int main() {
  // // 创建并调整文件大小
  // int fd = open(FILE_NAME, O_RDWR | O_CREAT | O_TRUNC, 0666);
  // if (fd == -1) {
  //     perror("open failed");
  //     return EXIT_FAILURE;
  // }
  // if (ftruncate(fd, FILE_SIZE) == -1) {
  //     perror("ftruncate failed");
  //     close(fd);
  //     return EXIT_FAILURE;
  // }

  // // 内存映射文件
  // char* map = mmap(NULL, FILE_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
  // if (map == MAP_FAILED) {
  //     perror("mmap failed");
  //     close(fd);
  //     return EXIT_FAILURE;
  // }

  // 初始化原子计数器
  // atomic_store(&g_offset, 0);
  InitMappingFile();

  // 创建线程
  pthread_t threads[NUM_THREADS];
  // ThreadArgs args = {map, FILE_SIZE, BLOCK_SIZE};

  for (int i = 0; i < NUM_THREADS; ++i) {
    // args.index = i;
    if (pthread_create(&threads[i], NULL, thread_write, NULL) != 0) {
      perror("pthread_create failed");
      break;
    }
  }

  // 等待线程完成
  for (int i = 0; i < NUM_THREADS; ++i) {
    if (pthread_join(threads[i], NULL) != 0) {
      perror("pthread_join failed");
    }
  }

  CleanMappingFile();

  // // 清理资源
  // if (munmap(map, FILE_SIZE) == -1) {
  //     perror("munmap failed");
  // }
  // close(fd);

  printf("All threads completed writing\n");
  return EXIT_SUCCESS;
}