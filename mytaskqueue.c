#include "mytaskqueue.h"
#include <stdlib.h>
#include <stdio.h>

//Implementation source: https://www.youtube.com/watch?v=FMNnusHqjpw


void init_queue_task(TaskQueue *queue){
    queue->head = NULL;
    queue->tail = NULL;
    queue->done = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->empty, NULL);
}

void enqueue_task(TaskQueue *queue, char* address, int size, int task_id){
    Task *task = malloc(sizeof(Task));
    task->chunk_address = address;
    task->chunk_size = size;
    task->task_id = task_id;
    task->next = NULL;
    pthread_mutex_lock(&queue->mutex); // Mutex lock so that we can add the task to the queue without a race 
    if(queue->head != NULL){
        queue->tail->next = task;
        queue->tail = task;
    } else{
        queue-> head = task;
        queue-> tail = task;
    }
    queue->task_count++;
    pthread_cond_signal(&queue->empty); // Signal to consumers that the queue is not empty anymore 
    pthread_mutex_unlock(&queue->mutex); // Unlock mutex so that other producers can access the queue 
}

Task* dequeue_task(TaskQueue *queue){
    pthread_mutex_lock(&queue->mutex); // Mutex lock so that we can delete the task of the queue without a race 
    while(queue->head == NULL && queue->done != 1){
        pthread_cond_wait(&queue->empty, &queue->mutex); //Threads should wait until there is a task to delete the task. Also, it unblocks
        //the queue in case any producer wants to use it, avoiding a deadlock
    }
    if(queue->head == NULL && queue->done == 1){
        pthread_mutex_unlock(&queue->mutex);
        return NULL;
    } 
        Task *task = queue->head;
        queue->head = queue->head->next;
        if(queue->head == NULL){
            queue->tail = NULL;
        }
        queue->task_count--;
        pthread_mutex_unlock(&queue->mutex);
        return task;
}

void free_task_queue(TaskQueue *queue){
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->empty);
}

