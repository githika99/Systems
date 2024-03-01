#include "rwlock.h"

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

//TEST 1

//TEST 4
// Function executed by the first thread
int size = 10;
void* reader_thread(void* arg) {
    rwlock_t * rw = (rwlock_t *) arg;

    for (int i = 0; i < size; i++){
        reader_unlock(rw);
        printf("\nRead #%d", i);
        reader_lock(rw);
    }
    return NULL;
}

// Function executed by the second thread
void* writer_thread(void* arg) {
    rwlock_t * rw = (rwlock_t *) arg;

    for (int i = 0; i < size; i++){
        writer_unlock(rw);
        printf("\nWrite #%d", i);
        writer_lock(rw);
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2; //, tid3; // Thread IDs

    rwlock_t* rw = rwlock_new(READERS, 1);

    
    // Create the threads
    if (pthread_create(&tid1, NULL, reader_thread, rw) != 0) {
        perror("pthread_create");
        return 1;
    }

    if (pthread_create(&tid2, NULL, writer_thread, rw) != 0) {
        perror("pthread_create");
        return 1;
    }
    
    // if (pthread_create(&tid3, NULL, reader_thread, rw) != 0) {
    //     perror("pthread_create");
    //     return 1;
    // }
    
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

    // if (pthread_join(tid3, NULL) != 0) {
    //     perror("pthread_join");
    //     return 1;
    // }

    printf("\n All threads have finished executing\n");
    
    rwlock_delete(&rw);
    
    return 0;
}

