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
#include <assert.h>
#include "queue.h"

typedef struct queue {
    void **buffer;
    int in, out, size;
    sem_t empty_cells;
    sem_t full_cells;
    sem_t mutex;

} queue;

//Dynamically allocates and initializes a new queue with a maximum size, size
//param size the maximum size of the queue
//return a pointer to a new queue_t
queue_t *queue_new(int size) {
    queue_t *q = malloc(sizeof(queue_t));
    if (q == NULL) {
        fprintf(stderr, "Malloc Failed");
        exit(1);
    }
    q->buffer = malloc(size * sizeof(void *));
    if (q->buffer == NULL) {
        fprintf(stderr, "Malloc Failed");
        exit(1);
    }
    q->in = 0;
    q->out = 0;
    q->size = size;
    sem_init(&(q->empty_cells), 0, size); //size empty cells when you start
    sem_init(&(q->full_cells), 0, 0);
    sem_init(&(q->mutex), 0, 1);
    return q;
}

//Delete your queue and free all of its memory.
void queue_delete(queue_t **q) {
    if (q != NULL && *q != NULL) {
        free((*q)->buffer);
        (*q)->buffer = NULL;
        sem_destroy(&((*q)->empty_cells));
        sem_destroy(&((*q)->full_cells));
        sem_destroy(&((*q)->mutex));
        free(*q);
        *q = NULL;
    }
}

//push an element onto a queue
//param q the queue to push an element into.
//param elem th element to add to the queue
//return A bool indicating success or failure.  Note, the function
//should succeed unless the q parameter is NULL.
bool queue_push(queue_t *q, void *elem) {
    if (q != NULL) {
        sem_wait(&(q->empty_cells)); //if full is at 0, no cause youre incrementing
        sem_wait(&(q->mutex));
        q->buffer[q->in] = elem;
        q->in = (q->in + 1) % (q->size);
        sem_post(&(q->mutex));
        sem_post(&(q->full_cells));
        return true;
    }
    return false;
}

//pop an element from a queue.
//param q the queue to pop an element from.
//param elem a place to assign the poped element.
//return A bool indicating success or failure.  Note, the function
//should succeed unless the q parameter is NULL.
bool queue_pop(queue_t *q, void **elem) {
    if (q != NULL) {
        sem_wait(&(q->full_cells));
        sem_wait(&(q->mutex));
        *elem = q->buffer[q->out];
        q->out = (q->out + 1) % (q->size);
        sem_post(&(q->mutex));
        sem_post(&(q->empty_cells));
        return true;
    }
    return false;
}
