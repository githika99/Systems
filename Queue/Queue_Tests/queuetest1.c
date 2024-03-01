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

#define THREADS 4

// The statistics that we're storing.
int counts[THREADS];

// the done signal
int done = 0;
pthread_mutex_t dmutex = PTHREAD_MUTEX_INITIALIZER;

// the reader/writer lock
queue_t *queue_d;

// sleep for a random amount of time. 
void random_sleep(void) { 
  unsigned r = rand() % 1000000;
  int rc = usleep(r);
  assert (!rc);
}

// safely see if we're done.
int get_done(void) {
  pthread_mutex_lock(&dmutex);
  int d = done;
  pthread_mutex_unlock(&dmutex);
  return d;
}

// safely say that we're done
void set_done(void) {
  pthread_mutex_lock(&dmutex);
  done = 1;
  pthread_mutex_unlock(&dmutex);
}

void* push_thread(void *args) {
  uintptr_t id = (uintptr_t)args;

  while (!get_done()) {
    time_t *timestamp = malloc(sizeof(time_t));
    if (timestamp == NULL) {
        fprintf(stderr, "Memory allocation failed for timestamp\n");
        return NULL;
    }
    *timestamp = (int)time(NULL);
    queue_push(queue_d, timestamp);
    printf("\nqueue %lu PUSHED: %ld\n", id, *timestamp);
    counts[id] ++;
  }

  printf("\npush_thread %lu exiting\n", id);
  return NULL;
}

void* pop_thread(void *args) {
  uintptr_t id = (uintptr_t)args;

   while (!get_done()) {
        time_t *timestamp;
        void* elem;
        if (queue_pop(queue_d, &elem)) {
            timestamp = (time_t *)elem;
            if (timestamp != NULL) { 
                printf("\nqueue %lu POPPED: %ld\n", id, *timestamp); // Use %ld to print time_t
                counts[id]++;
                free(timestamp);
            }
        }
    }

  printf("\npop_thread %lu exiting\n", id);
  return NULL;
}

int main() {
  pthread_t threads[THREADS];
  int rc = 0;
  uintptr_t sum = 0;

  queue_d = queue_new(10);
  pthread_create(threads , NULL, push_thread, (void*)0);
  pthread_create(threads + 1, NULL, push_thread, (void*)1);
  pthread_create(threads + 2, NULL, pop_thread, (void*)2);
  pthread_create(threads + 3, NULL, pop_thread, (void*)3);

  // sleep for some time. then cancel all of the threads
  sleep(100);
  set_done();

  for (int i = 0; i < THREADS; ++i) {
    pthread_join(threads[i], NULL);
    sum += counts[i];
  }

  queue_delete(&queue_d);

  for (int i = 0; i < THREADS; ++i) {
    fprintf(stderr, "%i: %i (%lf)\n", i, counts[i], (double)counts[i] / (double)sum);
  }

  assert (!rc);
  return 0;
}
