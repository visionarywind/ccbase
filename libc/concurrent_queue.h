#pragma once

#include <pthread.h>


typedef void *(*malloc_libc)(size_t);
// extern "C" malloc_libc real_malloc;
typedef void (*free_libc)(void *ptr);
// extern "C" free_libc real_free;

typedef struct Node {
    void *data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    pthread_mutex_t head_mutex;
    pthread_mutex_t tail_mutex;
    pthread_cond_t cond;

    malloc_libc malloc_func;
    free_libc free_func;
} ConcurrentQueue;



// 初始化队列
extern "C" void concurrent_queue_init(ConcurrentQueue *q, malloc_libc malloc_func, free_libc free_func);

// 销毁队列
extern "C" void concurrent_queue_destroy(ConcurrentQueue *q);

// 入队操作
extern "C" void concurrent_queue_enqueue(ConcurrentQueue *q, void *data);

// 出队操作（阻塞版）
extern "C" void *concurrent_queue_dequeue(ConcurrentQueue *q);

// 非阻塞尝试出队
extern "C" int concurrent_queue_try_dequeue(ConcurrentQueue *q, void **result);