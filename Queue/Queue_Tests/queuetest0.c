#include "queue.h"
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

//struct for passing arguments to thread_function1 and thread_function2
 struct ThreadArgs {
        queue_t* q;
        int size;
    };

// Function executed by the first thread
void* thread_function1(void* arg) {
    struct ThreadArgs* args = (struct ThreadArgs*)arg;
    queue_t* q = args->q;
    int size = args->size;

    for (int i = 0; i < 2*size; i++){
        queue_push(q, &i);
    }
    return NULL;
}

// Function executed by the second thread
void* thread_function2(void* arg) {
  
    struct ThreadArgs* args = (struct ThreadArgs*)arg;
    queue_t* q = args->q;
    int size = args->size;

    for (int i = 0; i < size; i++){
        void** j = malloc(sizeof(void*)); 
        if (j == NULL){
            printf("malloc failed");
            break;
        }
        queue_pop(q, j);
        free(j);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2, tid3; // Thread IDs

    int size = 5;
    queue_t *q = queue_new(size);

    struct ThreadArgs args = { q, size };
    
    // Create the first thread
    if (pthread_create(&tid2, NULL, thread_function1, &args) != 0) {
        perror("pthread_create");
        return 1;
    }

    if (pthread_create(&tid3, NULL, thread_function2, &args) != 0) {
        perror("pthread_create");
        return 1;
    }
    
    // Create the second thread
    if (pthread_create(&tid1, NULL, thread_function2, &args) != 0) {
        perror("pthread_create");
        return 1;
    }
    
    // Wait for the first thread to finish
    if (pthread_join(tid1, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }
    
    // Wait for the second thread to finish
    if (pthread_join(tid2, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    if (pthread_join(tid3, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    printf("\nBoth threads have finished executing\n");
    
    // for (int i = 0; i < size; i++){
    //     queue_push(q, &i);
    // }

    // for (int i = 0; i < size; i++){
    //     void** j = malloc(sizeof(void*)); 
    //     queue_pop(q, j);
    //     printf("\npopped: %d", **(int**)j);
    //     free(j);
    // }
    
    queue_delete(&q);
    
    return 0;
}

/*
//Notes to make a complex queue test:
- more threads 
*/





