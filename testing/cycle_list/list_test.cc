#include <iostream>
#include <memory>

#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// tracing frame levels, deeper level means more performance reduction.
#define MAX_STACK_FRAMES 256
typedef struct {
  void *buffer[MAX_STACK_FRAMES];
  int frames;
  void *ptr;
  char ops_type;
  size_t size;
} ExecStack;

typedef struct Node {
    struct Node *next;
    struct Node *prev;
    ExecStack exec_stack;
    int index;
} Node;

#define EXEC_STACK_NODE_SIZE (sizeof(Node))


typedef struct {
    Node head;

    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
} BlockingQueue;


BlockingQueue kBlockingQueue;

void PrintQueue(BlockingQueue *queue) {
    Node *current = queue->head.next;
    while (current != &queue->head) {
        printf("%d,", current->index);
        current = current->next;
    }
    printf("\n");
}

void RPrintQueue(BlockingQueue *queue) {
    Node *current = queue->head.prev;
    while (current != &queue->head) {
        printf("%d,", current->index);
        current = current->prev;
    }
    printf("\n");
}

void InitQueue(BlockingQueue *queue) {
    queue->head.next = queue->head.prev = &queue->head;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
}

void Enqueue(BlockingQueue *queue, Node *node) {
    pthread_mutex_lock(&queue->mutex);
    node->prev = &queue->head;
    node->next = queue->head.next;
    queue->head.next->prev = node;
    queue->head.next = node;

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
}

Node *Dequeue(BlockingQueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->head.prev == &queue->head) {
        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }

    Node *node = queue->head.prev;
    node->prev->next = &queue->head;
    queue->head.prev = node->prev;
    pthread_mutex_unlock(&queue->mutex);
    return node;
}

void DestroyQueue(BlockingQueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    Node *current = queue->head.next;
    while (current != NULL) {
        Node *next = current->next;
        current = next;
    }
    pthread_mutex_unlock(&queue->mutex);
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
}

// ConcurrentQueue kConcurrentQueue;
volatile bool init_flag = true;

struct Info {
  char *p1;
  unsigned char *p2;
};

int main() {
  InitQueue(&kBlockingQueue);
  for (int i = 0; i < 100; i++) {
    Node *node = new Node();
    node->index = i;
    Enqueue(&kBlockingQueue, node);
    printf("Enqueue %d\n", i);
  }
  
  Node *node = Dequeue(&kBlockingQueue);
  cout << "Dequeue : " << node->index << endl;
  node = Dequeue(&kBlockingQueue);
  cout << "Dequeue : " << node->index << endl;
  node = Dequeue(&kBlockingQueue);
  cout << "Dequeue : " << node->index << endl;

  node = new Node();
  node->index = 0;
  Enqueue(&kBlockingQueue, node);
  printf("Enqueue %d\n", node->index);

  PrintQueue(&kBlockingQueue);
  RPrintQueue(&kBlockingQueue);
  unsigned char *p = (unsigned char *)0xfffd0cbff000;
  printf("p: %p\n", p);
  p += 2197815340;
  printf("p: %p\n", p);

  cout << sizeof(Info) << endl;
  Info *pInfo = new Info();
  void *ptr = malloc(2197815340);
  pInfo->p2 = (unsigned char *)ptr;
  printf("before: %p\n", pInfo->p2);
  pInfo->p2 += 2197815340;
  printf("after: %p\n", pInfo->p2);
  return 0;
}