#include "myresultqueue.h"
#include <stdlib.h>

//Implementation source: https://www.youtube.com/watch?v=FMNnusHqjpw

void init_queue_result(ResultQueue *queue){
    queue->head = NULL;
    queue->tail = NULL;
    queue->result_num = 0;
    queue->done = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->empty, NULL);
}

void enqueue_result(ResultQueue *queue, char* character, int result_id, int size){
    Result *result = malloc(sizeof(Result));
    result->encoded = character;
    result->result_id = result_id;
    result->size = size;
    result->next = NULL;

    pthread_mutex_lock(&queue->mutex); // Mutex lock so that we can add the task to the queue without a race 
    if(queue->head != NULL){
        queue->tail->next =result;
        queue->tail = result;
    } else{
        queue-> head = result; queue -> tail = result;
    }
    queue->result_num++;

    pthread_mutex_unlock(&queue->mutex); // Unlock mutex so that other producers can access the queue 
}

Result* dequeue_result(ResultQueue *queue){
    pthread_mutex_lock(&queue->mutex); // Mutex lock so that we can delete the task of the queue without a race 
    while(queue->head == NULL && queue->done != 1){
        pthread_cond_wait(&queue->empty, &queue->mutex); //Threads should wait until there is a task to delete the task. Also, it unblocks
        //the queue in case any producer wants to use it, avoiding a deadlock
    }
    if(queue->head == NULL && queue->done == 1){
        pthread_mutex_unlock(&queue->mutex);
        return NULL;
    } 
        Result *result = queue->head;
        queue->head = queue->head->next;
        if(queue->head == NULL){
            queue->tail = NULL;
        }
        queue->result_num--;
        pthread_mutex_unlock(&queue->mutex);
        return result;
    
}

void free_result_queue(ResultQueue *queue){
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->empty);
}


