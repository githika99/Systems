/*
Githika Annapureddy
gannapur
Asgn 3
*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "rwlock.h"

typedef struct rwlock {
    int waiting_readers;
    int waiting_writers;
    int active_readers;
    int active_writers;
    int signal;
    PRIORITY PRIORITY;
    int n;
    int n_readers;
    pthread_cond_t
        reader; //reader waits for this CV when a writer has the thread. This is signaled in writer_unlock()
    pthread_cond_t writer;
    pthread_mutex_t writer_lock;
    pthread_mutex_t lock;
} rwlock;

//user-defined type called PRIORITY that is an enum type
//0 now corresponds to READERS, 1 corresponds to WRITERS, and 2 corresponds to N_WAY

//Dynamically allocates and initializes a new rwlock
rwlock_t *rwlock_new(PRIORITY p, uint32_t n) {
    rwlock_t *rw = malloc(sizeof(rwlock_t));
    if (rw == NULL) {
        fprintf(stderr, "malloc failed");
        exit(1);
    }
    rw->waiting_readers = 0;
    rw->waiting_writers = 0;
    rw->active_readers = 0;
    rw->active_writers = 0;
    rw->signal = 0;
    rw->PRIORITY = p;
    rw->n_readers = 0;
    rw->n = (int) n;
    if (p == N_WAY && rw->n < 1) {
        free(rw);
        rw = NULL;
        fprintf(stderr, "Incorrect value of n");
        exit(1);
    }
    int rc = 0;
    //writer lock and lock are used as a resource. Should be initialized to 1 if it was a sem. Only one thread should have them at a time. (sem_wait should call them first and any subsequent sem_waits (before the sem_post) should be stopped)
    //wait_for_writer and reader_done are used for ordering. Should be initialized to 0 if it was a sem.
    rc += pthread_mutex_init(&(rw->lock), NULL);
    rc += pthread_cond_init(&(rw->writer), NULL);
    rc += pthread_cond_init(&(rw->reader), NULL);
    rc += pthread_mutex_init(&(rw->writer_lock), NULL);
    if (rc != 0) {
        free(rw);
        rw = NULL;
        fprintf(stderr, "mutex init failed");
        exit(1);
    }
    return rw;
}

//Delete your rwlock and free all of its memory.
void rwlock_delete(rwlock_t **rw) {
    if (rw != NULL && *rw != NULL) {
        int rc = 0;
        rc += pthread_mutex_destroy(&((*rw)->lock));
        rc += pthread_cond_destroy(&((*rw)->writer));
        rc += pthread_cond_destroy(&((*rw)->reader));
        rc += pthread_mutex_destroy(&((*rw)->writer_lock));
        free(*rw);
        *rw = NULL;
        if (rc != 0) {
            fprintf(stderr, "mutex destroy failed because: %s\n", strerror(rc));
            exit(1);
        }
    }
}

// int id = 0; //DELETE LATER OR ELSEEEEE

//acquire rw for reading
void reader_lock(rwlock_t *rw) {
    pthread_mutex_lock(&(rw->lock));
    while (rw->signal == 1) {
        pthread_mutex_unlock(&(rw->lock));
        pthread_mutex_lock(&(rw->lock));
    }
    // int my_id = id++;
    rw->waiting_readers++;
    // printf("\n\nEntered reader lock, id:%d", my_id);
    // printf("\nwaiting_readers is now: %d", rw->waiting_readers); fflush(stdout);
    if (rw->PRIORITY == 0) { //PRIORITY is READER
        if (rw->active_writers == 1) { //wait if writer has lock
            // printf("\nReader waiting for: reader, id:%d", my_id);
            pthread_cond_wait(&(rw->reader), &(rw->lock));
            rw->signal = 0;
            // printf("\nReader got  reader, id:%d", my_id);

            if (rw->active_readers == 0) { //first reader aquires writer lock
                // printf("\nReader waiting for: writer_lock, id:%d", my_id);
                pthread_mutex_lock(&(rw->writer_lock));
                // printf("\nReader got writer_lock, id:%d", my_id);
            }
        } else if (rw->active_readers == 0
                   && rw->waiting_readers
                          == 1) { //if writer_lock is free AND this is the first read
            // printf("\nReader waiting for: writer_lock, id:%d", my_id);
            pthread_mutex_lock(&(rw->writer_lock));
            // printf("\nReader got writer_lock, id:%d", my_id);
        }
    }

    else if (rw->PRIORITY == 1) { //PRIORITY is READER
        if (rw->waiting_writers > 0
            || rw->active_writers == 1) { //if writer has lock or a writer is waiting for the lock
            //printf("\nReader waiting for: reader, id:%d", my_id);
            pthread_cond_wait(&(rw->reader), &(rw->lock));
            rw->signal = 0;
            //printf("\nReader got reader, id:%d", my_id);
            if (rw->active_readers == 0) { //first reader aquires writer lock
                //printf("\nReader waiting for: writer_lock, id:%d", my_id);
                pthread_mutex_lock(&(rw->writer_lock));
                //printf("\nReader got writer_lock, id:%d", my_id);
            }
        } else if (rw->active_readers == 0
                   && rw->waiting_readers
                          == 1) { //if writer_lock is free AND this is the first read
            //printf("\nReader waiting for: writer_lock, id:%d", my_id);
            pthread_mutex_lock(&(rw->writer_lock));
            //printf("\nReader got writer_lock, id:%d", my_id);
        }
    }

    else if (rw->PRIORITY == 2) { //N-WAY
        if (rw->active_writers == 1
            || rw->n_readers
                   == rw->n) { //wait loop if writer has lock OR reader has lock but all n spots taken could do this in 2nd half of OR: rw->waiting_readers + rw->n_readers > rw->n
            while (1) {
                //printf("\nReader waiting for: reader, id:%d", my_id);
                pthread_cond_wait(&(rw->reader), &(rw->lock));
                rw->signal = 0;
                //printf("\nReader got reader, id:%d", my_id);
                if (rw->n_readers < rw->n) {
                    rw->n_readers++;
                    break;
                }
            }
        } else if (rw->active_writers
                   == 0) { //writer does not have lock and all waiting_readers can aquire lock
            //printf("\nIncrememnt n_readers, id:%d", my_id);
            rw->n_readers++;
        }
        if (rw->active_readers == 0) { //first of n reads gets writer lock OR do rw->n_readers == 1
            //printf("\nReader waiting for: writer_lock, id:%d", my_id);
            pthread_mutex_lock(&(rw->writer_lock));
            //printf("\nReader got  writer_lock, id:%d", my_id);
        }
    }

    //printf("\nproceeding with read, n_readers is now %d", rw->n_readers);
    rw->waiting_readers--;
    rw->active_readers++;
    pthread_mutex_unlock(&(rw->lock));
}

//release rw for reading
//note: reader_unlock can never give the reader theread the lock in READERS or WRITERS
void reader_unlock(rwlock_t *rw) {
    pthread_mutex_lock(&(rw->lock));
    while (rw->signal == 1) {
        pthread_mutex_unlock(&(rw->lock));
        pthread_mutex_lock(&(rw->lock));
    }
    // int my_id = id++;
    // printf("\n\nEntered reader_unlock, id:%d", my_id); fflush(stdout);

    if (rw->active_readers == 0) { //if reader_unlock called before reader_lock
        // printf("\nReader mistakenly aquired lock, id:%d", my_id); fflush(stdout);
        pthread_mutex_unlock(&(rw->lock));
        return;
    }

    rw->active_readers--;
    //printf("\n Read done, waiting_readers is now: %d, active_readers: %d", rw->waiting_readers, rw->active_readers);
    //fflush(stdout);

    if (rw->PRIORITY == 0 && rw->active_readers == 0
        && rw->waiting_readers == 0) { //last reader releases lock if PRIORITY is READERS
        if (rw->waiting_writers > 0) { //if writer is waiting for lock, signal
            // printf("\nReader signaled: writer, id:%d", my_id); fflush(stdout);
            pthread_cond_signal(&(rw->writer));
            rw->signal = 1;
        }
        // printf("\nReader unlocked writer_lock, id:%d", my_id); fflush(stdout);
        pthread_mutex_unlock(&(rw->writer_lock));
    } else if (rw->PRIORITY == 1) {
        if (rw->waiting_writers > 0
            && rw->active_readers == 0) { //give to writers if all readers done
            //printf("\nReader signaled: writer, id:%d", my_id);
            pthread_cond_signal(&(rw->writer));
            rw->signal = 1;
        }
        if (rw->active_readers == 0) { //release lock if all readers are done
            //printf("\nReader unlocked writer_lock, id:%d", my_id);
            pthread_mutex_unlock(&(rw->writer_lock));
        }
    }

    else if (rw->PRIORITY == 2) {
        if (rw->active_readers == 0) {
            rw->n_readers = 0;
            if (rw->waiting_writers > 0) {
                //printf("\nReader signaled: writer, id:%d", my_id);
                pthread_cond_signal(&(rw->writer));
                rw->signal = 1;
            } else if (rw->waiting_readers > 0) {
                //printf("\nReader signaled: reader, id:%d", my_id);
                pthread_cond_broadcast(&(rw->reader));
                rw->signal = 1;
            }
            //printf("\nReader unlocked writer_lock, id:%d", my_id);
            pthread_mutex_unlock(&(rw->writer_lock));
        }
    }

    pthread_mutex_unlock(&(rw->lock));
}

//acquire rw for writing
void writer_lock(rwlock_t *rw) {
    pthread_mutex_lock(&(rw->lock));
    while (rw->signal == 1) {
        pthread_mutex_unlock(&(rw->lock));
        pthread_mutex_lock(&(rw->lock));
    }
    // int my_id = id++;
    rw->waiting_writers++;
    // printf("\n\nEntered Writer Lock, id:%d", my_id);
    // printf("\nwaiting_writers is now: %d, active_writers is now: %d, waiting_readers is now: %d, active_readers is now: %d,", rw->waiting_writers,rw->active_writers, rw->waiting_readers, rw->active_readers); fflush(stdout);
    if (rw->PRIORITY == 0) {
        if (rw->active_writers == 1 || rw->waiting_readers > 0 || rw->active_readers > 0
            || rw->waiting_writers > 1) { //lock is in use or waiting writer or waiting reader
            // printf("\nWriter waiting for: writer, id:%d", my_id); fflush(stdout);
            pthread_cond_wait(&(rw->writer), &(rw->lock));
            rw->signal = 0;
            // printf("\nWriter got writer, id:%d", my_id); fflush(stdout);
        }
        // printf("\nWriter waiting for: writer_lock, id:%d", my_id); fflush(stdout);
        pthread_mutex_lock(&(rw->writer_lock));
        // printf("\nWriter got writer_lock, id:%d", my_id); fflush(stdout);
    } else if (rw->PRIORITY == 1) { //Priority is writers, get lock once writer is done
        if (rw->active_writers == 1 || rw->active_readers > 0
            || rw->waiting_writers > 1) { //lock is in use or there is a writer waiting
            //printf("\nWriter waiting for: writer, id:%d", my_id);
            pthread_cond_wait(&(rw->writer), &(rw->lock));
            rw->signal = 0;
            //printf("\nWriter got writer, id:%d", my_id);
        }
        //printf("\nWriter waiting for: writer_lock, id:%d", my_id);
        pthread_mutex_lock(&(rw->writer_lock));
        //printf("\nWriter got writer_lock, id:%d", my_id);
    }

    else if (rw->PRIORITY == 2) { //Priority is N-WAY
        if (rw->active_writers == 1 || rw->active_readers > 0
            || rw->waiting_writers > 1) { //lock is in use or there is a writer waiting
            //printf("\nWriter waiting for: writer, id:%d", my_id);
            pthread_cond_wait(&(rw->writer), &(rw->lock));
            rw->signal = 0;
            //printf("\nWriter got writer, id:%d", my_id);
        }
        //printf("\nWriter waiting for: writer_lock, id:%d", my_id);
        pthread_mutex_lock(&(rw->writer_lock));
        //printf("\nWriter got writer_lock, id:%d", my_id);
    }

    // printf("\nproceeding with write");
    rw->waiting_writers--;
    rw->active_writers++;
    pthread_mutex_unlock(&(rw->lock));
}

//release rw for writing
void writer_unlock(rwlock_t *rw) {
    pthread_mutex_lock(&(rw->lock));
    while (rw->signal == 1) {
        pthread_mutex_unlock(&(rw->lock));
        pthread_mutex_lock(&(rw->lock));
    }
    // int my_id = id++;
    // printf("\n\nEntered writer unlock, id:%d", my_id);
    if (rw->active_writers == 0) { //if reader_unlock called before reader_lock
        // printf("\nwriter mistakenly aquired lock, id:%d", my_id); fflush(stdout);
        pthread_mutex_unlock(&(rw->lock));
        return;
    }
    rw->active_writers--;
    //printf("\n Write done, active_writers is now: %d", rw->active_writers);
    //fflush(stdout);

    if (rw->PRIORITY == 0) { //READERS
        if (rw->waiting_readers > 0) {
            // printf("\nWriter signaled: reader, id:%d", my_id);
            pthread_cond_broadcast(&(rw->reader));
            rw->signal = 1;
        } else if (rw->waiting_writers > 0) {
            // printf("\nWriter signaled: writer, id:%d", my_id);
            pthread_cond_signal(&(rw->writer));
            rw->signal = 1;
        }
        // printf("\nWriter unlocked writer_lock, id:%d", my_id);
        pthread_mutex_unlock(&(rw->writer_lock));
    }

    else if (rw->PRIORITY == 1) { //WRITERS
        if (rw->waiting_writers > 0) {
            //printf("\nWriter signaled: writer, id:%d", my_id);
            pthread_cond_signal(&(rw->writer));
            rw->signal = 1;
        } else if (rw->waiting_readers > 0) {
            //printf("\nWriter signaled: reader, id:%d", my_id);
            pthread_cond_broadcast(&(rw->reader));
            rw->signal = 1;
        }
        //printf("\nWriter unlocked writer_lock, id:%d", my_id);
        pthread_mutex_unlock(&(rw->writer_lock));
    }

    else if (rw->PRIORITY == 2) {
        rw->n_readers = 0;
        if (rw->waiting_readers > 0) {
            //printf("\nWriter signaled: reader, id:%d", my_id);
            pthread_cond_broadcast(&(rw->reader));
            rw->signal = 1;
        } else if (rw->waiting_writers > 0) {
            //printf("\nWriter signaled: writer, id:%d", my_id);
            pthread_cond_signal(&(rw->writer));
            rw->signal = 1;
        }
        //printf("\nWriter unlocked writer_lock, id:%d", my_id);
        pthread_mutex_unlock(&(rw->writer_lock));
    }

    pthread_mutex_unlock(&(rw->lock));
}
