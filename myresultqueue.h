#ifndef MYRESULTQUEUE_H_
#define MYRESULTQUEUE_H_

#include <pthread.h>


typedef struct Result{
    char* encoded;
    int result_id;
    struct Result* next;
} Result;

typedef struct ResultQueue{
    Result* head;
    Result* tail;
    int result_num;
    pthread_mutex_t mutex;
    pthread_cond_t empty;
}ResultQueue;

void init_queue_result(ResultQueue* queue);
void enqueue_result(ResultQueue* queue, char* character, int id);
int is_result_queue_empty(ResultQueue* queue);
Result* dequeue_result(ResultQueue* queue);
void free_result_queue(ResultQueue *queue);


#endif /* MYRESULTQUEUE_H_ */