#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include "mytaskqueue.h"
#include <string.h>
#include "myresultqueue.h"
#include <stdbool.h>

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

TaskQueue task_queue;
ResultQueue result_queue;

int task_id = 0;

int total_results = 0;
pthread_mutex_t encoded_mutex = PTHREAD_MUTEX_INITIALIZER; 

void *encoder_multithreaded()
{
    while(1){
        pthread_mutex_lock(&encoded_mutex);
        Task *task = dequeue_task(&task_queue);
        if(task == NULL){
            pthread_mutex_unlock(&encoded_mutex);
        } else{
            int size = 0;
            char* encoded = malloc(sizeof(char*) * 20000);
            char* addr = task->chunk_address;
            unsigned int char_count = 0;
            int index = 0;
            char prev_char = addr[0];
            for(int i = 0; i < task->chunk_size; i++){
                if(addr[i] != prev_char){
                   encoded[index++] = prev_char;
                   encoded[index++] = char_count;
                   size+=2;
                   prev_char = addr[i];
                   char_count = 1;
                } else{
                    char_count++;
                }
            }
            encoded[index++] = prev_char;
            encoded[index++] = char_count;
            size +=2;
            enqueue_result(&result_queue, encoded, task->task_id, size);
            pthread_mutex_unlock(&encoded_mutex);
        }
        } 
        return NULL;
    }

// Multi-threaded encoder
int main(int argc, char *argv[])
{

    int output = STDOUT_FILENO;

    // Getting the number of threads https://linux.die.net/man/3/getopt
    int thread_count = 1;
    int opt;
    while ((opt = getopt(argc, argv, "j:")) != -1)
    {
        switch (opt)
        {
        case 'j':
            thread_count = atoi(optarg);
            break;
        case '?':
            handle_error("opt");
        }
    }
    // Initiliaze result queue
    init_queue_task(&task_queue);
    init_queue_result(&result_queue);

    
    pthread_t *threads = malloc(thread_count * sizeof(pthread_t));

    for (int i = 0; i < thread_count; i++)
    {
        if (pthread_create(&threads[i], NULL, encoder_multithreaded, NULL) != 0)
        {
            handle_error("pthread_create");
        }
    }

    char last_char = 0;
    int last_count = 0;
    // Looping through all input files
    for (int i = optind; i < argc; i++)
    {
        int fd = open(argv[i], O_RDONLY);
        if (fd == -1)
        {
            handle_error("fd");
        }

        // Get file size
        struct stat sb;
        if (fstat(fd, &sb) == -1)
        {
            handle_error("sb");
        }

        // Map file into memory
        char *addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (addr == MAP_FAILED)
        {
            handle_error("addr");
        }

        if (thread_count > 1)
        {
            int chunk_size = 4096; // 4KB

            int chunk_count = sb.st_size / chunk_size + (sb.st_size % chunk_size != 0);

            total_results += chunk_count;

            char *mem_address;
             
            for (int j = 0; j < chunk_count; j++)
            {
                mem_address = addr + j * chunk_size;
                if (j == chunk_count - 1)
                {
                    chunk_size = sb.st_size - j * chunk_size;
                }
                enqueue_task(&task_queue, mem_address, chunk_size, task_id++);
            }
        }
        else
        {
            // Checking the # of occurances of each char and writing it to stdout
            int char_count = 1;
            char prev_char = addr[0];

            if (prev_char == last_char && i > 1)
            {
                char_count += last_count;
            }
            for (int j = 1; j < sb.st_size; j++)
            {
                if (addr[j] != prev_char)
                {
                    write(output, &prev_char, 1);
                    write(output, &char_count, 1);
                    char_count = 1;
                    prev_char = addr[j];
                }
                else
                {
                    char_count++;
                }
            }
            last_char = prev_char;
            last_count = char_count;
            if (i == argc - 1)
            {
                write(output, &prev_char, 1);
                write(output, &char_count, 1);
            }
            // Closing fd and freeing memory https://linux.die.net/man/2/munmap
            close(fd);
            munmap(addr, sb.st_size);
        }
    }
    if (thread_count > 1)
        { 
            int processed_results = 0;
            Result **all_results = malloc(sizeof(Result) * 250000);
            int result_index = 0;
            int index = 0;
            char last_char;
            unsigned int last_count = 0;
            int exit = 0;
            while(1){
                if(processed_results == total_results){
                    result_queue.done = 1;
                }
                while(all_results[result_index] == NULL){
                     Result *result = dequeue_result(&result_queue); 
                     if(result == NULL){
                        exit = 1;
                        break;
                     } else{
                        index = result->result_id;
                        all_results[index] = result;
                    }
                }
                if(exit){
                    break;
                }
                Result *to_process = all_results[result_index];
                char* encoded = to_process->encoded;
               int len = to_process->size;
                if(index == 0){
                    last_char = encoded[len - 2];
                    last_count = encoded[len - 1];
                    write(output, encoded, len - 1);
                } else{
                    if(encoded[0] == last_char){
                        unsigned int sum = last_count + encoded[1];
                        if(len > 2){
                            write(output, &sum, 1);
                            write(output, encoded + 2, len - 3);
                            last_char = encoded[len - 2];
                            last_count = encoded[len - 1];
                        } else{
                            last_char = encoded[len - 2];
                            last_count = sum;
                        }
                    } else{
                        write(output, &last_count, 1);
                        write(output, encoded, len - 1);
                        last_char = encoded[len - 2];
                        last_count = encoded[len - 1];
                    }
                }
                if(index == total_results - 1){
                    write(output, &last_count, 1);
                }
                result_index++;
                processed_results++;
            }
        }
}
