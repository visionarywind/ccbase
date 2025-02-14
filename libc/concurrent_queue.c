#include "concurrent_queue.h"

#include <stdlib.h>

// 初始化队列
void concurrent_queue_init(ConcurrentQueue *q, malloc_libc malloc_func, free_libc free_func) {
    q->malloc_func = malloc_func;
    q->free_func = free_func;
    Node *dummy = (Node *)q->malloc_func(sizeof(Node));
    dummy->data = NULL;
    dummy->next = NULL;
    q->head = dummy;
    q->tail = dummy;
    pthread_mutex_init(&q->head_mutex, NULL);
    pthread_mutex_init(&q->tail_mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

// 销毁队列
void concurrent_queue_destroy(ConcurrentQueue *q) {
    pthread_mutex_lock(&q->head_mutex);
    pthread_mutex_lock(&q->tail_mutex);
    
    Node *current = q->head;
    while (current) {
        Node *temp = current;
        current = current->next;
        q->free_func(temp);
    }
    
    pthread_mutex_unlock(&q->tail_mutex);
    pthread_mutex_unlock(&q->head_mutex);
    pthread_mutex_destroy(&q->head_mutex);
    pthread_mutex_destroy(&q->tail_mutex);
    pthread_cond_destroy(&q->cond);
}

// 入队操作
void concurrent_queue_enqueue(ConcurrentQueue *q, void *data) {
    Node *node = (Node *)q->malloc_func(sizeof(Node));
    node->data = data;
    node->next = NULL;

    pthread_mutex_lock(&q->tail_mutex);
    q->tail->next = node;  // 将新节点链接到当前尾节点
    q->tail = node;        // 更新尾指针
    pthread_mutex_unlock(&q->tail_mutex);
    
    // 通知可能等待的消费者
    pthread_cond_signal(&q->cond);
}

// 出队操作（阻塞版）
void *concurrent_queue_dequeue(ConcurrentQueue *q) {
    pthread_mutex_lock(&q->head_mutex);
    
    // 使用循环处理虚假唤醒
    while (q->head->next == NULL) {
        pthread_cond_wait(&q->cond, &q->head_mutex);
    }
    
    Node *dummy = q->head;
    Node *result_node = dummy->next;  // 真正的数据节点
    void *result = result_node->data;
    
    q->head = result_node;  // 更新头指针
    pthread_mutex_unlock(&q->head_mutex);
    
    q->free_func(dummy);  // 释放旧的虚节点
    return result;
}

// 非阻塞尝试出队
int concurrent_queue_try_dequeue(ConcurrentQueue *q, void **result) {
    pthread_mutex_lock(&q->head_mutex);
    
    if (q->head->next == NULL) {
        pthread_mutex_unlock(&q->head_mutex);
        return 0;  // 队列为空
    }
    
    Node *dummy = q->head;
    Node *result_node = dummy->next;
    *result = result_node->data;
    
    q->head = result_node;
    pthread_mutex_unlock(&q->head_mutex);
    
    q->free_func(dummy);
    return 1;  // 成功取出数据
}