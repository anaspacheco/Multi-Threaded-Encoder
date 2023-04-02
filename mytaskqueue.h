#ifndef MYTASKQUEUE_H_
#define MYTASKQUEUE_H_

#include <pthread.h>

typedef struct Task{
    char* chunk_address;
    int chunk_size;
    int task_id;
    struct Task* next;
} Task;

typedef struct TaskQueue{
    Task* head;
    Task* tail;
     int done;
    int task_count;
    pthread_mutex_t mutex;
    pthread_cond_t empty;
}TaskQueue;

void init_queue_task(TaskQueue *queue);
void enqueue_task(TaskQueue *queue, char* address, int size, int id);
Task* dequeue_task(TaskQueue *queue);
int is_task_queue_empty(TaskQueue* queue);
void free_task_queue(TaskQueue *queue);

#endif /* MYTASKQUEUE_H_ */