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
#define THREADS 32

// The statistics that we're storing.
int counts[THREADS];

// the done signal
int done = 0;
pthread_mutex_t dmutex = PTHREAD_MUTEX_INITIALIZER;

// // the reader/writer lock
 rwlock_t*rwlock_d;

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

void* reader_thread(void *args) {
  uintptr_t id = (uintptr_t)args;

  while (!get_done()) {
    reader_lock(rwlock_d);
    fprintf(stderr, "reader %lu got lock\n", id);
    counts[id] ++;
    random_sleep();
    fprintf(stderr, "reader %lu done\n", id);
    reader_unlock(rwlock_d);
  }

  fprintf(stderr, "reader %lu exiting\n", id);
  return NULL;
}

void* writer_thread(void *args) {
  uintptr_t id = (uintptr_t)args;

  while (!get_done()) {
    writer_lock(rwlock_d);
    fprintf(stderr, "#writer %lu got lock\n", id);
    counts[id] ++;
    random_sleep();
    fprintf(stderr, "&writer %lu done\n", id);
    writer_unlock(rwlock_d);
  }

  fprintf(stderr, "writer %lu exiting\n", id);
  return NULL;
}

int main() {
  printf("\nNEW FUNCT BABY");
  pthread_t threads[THREADS];
  int rc = 0;
  uintptr_t sum = 0;

  rwlock_d = rwlock_new(N_WAY, 100); 
  pthread_create(threads , NULL, reader_thread, (void*)0);
  pthread_create(threads + 1, NULL, reader_thread, (void*)1);
  pthread_create(threads + 2, NULL, reader_thread, (void*)2);
  pthread_create(threads + 3, NULL, reader_thread, (void*)3);
  pthread_create(threads + 4 , NULL, reader_thread, (void*)4);
  pthread_create(threads + 5, NULL, reader_thread, (void*)5);
  pthread_create(threads + 6, NULL, reader_thread, (void*)6);
  pthread_create(threads + 7, NULL, reader_thread, (void*)7);
  pthread_create(threads + 8, NULL, reader_thread, (void*)8);
  pthread_create(threads + 9, NULL, reader_thread, (void*)9);
  pthread_create(threads + 10, NULL, reader_thread, (void*)10);
  pthread_create(threads + 11, NULL, reader_thread, (void*)11);
  pthread_create(threads + 12 , NULL, reader_thread, (void*)12);
  pthread_create(threads + 13, NULL, reader_thread, (void*)13);
  pthread_create(threads + 14, NULL, reader_thread, (void*)14);
  pthread_create(threads + 15, NULL, reader_thread, (void*)15);
  pthread_create(threads + 16, NULL, writer_thread, (void*)16);
  pthread_create(threads + 17, NULL, writer_thread, (void*)17);
  pthread_create(threads + 18, NULL, writer_thread, (void*)18);
  pthread_create(threads + 19, NULL, writer_thread, (void*)19);
  pthread_create(threads + 20 , NULL, writer_thread, (void*)20);
  pthread_create(threads + 21, NULL, writer_thread, (void*)21);
  pthread_create(threads + 22, NULL, writer_thread, (void*)22);
  pthread_create(threads + 23, NULL, writer_thread, (void*)23);
  pthread_create(threads + 24, NULL, writer_thread, (void*)24);
  pthread_create(threads + 25, NULL, writer_thread, (void*)25);
  pthread_create(threads + 26, NULL, writer_thread, (void*)26);
  pthread_create(threads + 27, NULL, writer_thread, (void*)27);
  pthread_create(threads + 28 , NULL, writer_thread, (void*)28);
  pthread_create(threads + 29, NULL, writer_thread, (void*)29);
  pthread_create(threads + 30, NULL, writer_thread, (void*)30);
  pthread_create(threads + 31, NULL, writer_thread, (void*)31);

  // sleep for some time. then cancel all of the threads
  sleep(100);
  set_done();

  for (int i = 0; i < THREADS; ++i) {
    pthread_join(threads[i], NULL);
    sum += counts[i];
  }

  rwlock_delete(&rwlock_d);

  for (int i = 0; i < THREADS; ++i) {
    fprintf(stderr, "%i: %i (%lf)\n", i, counts[i], (double)counts[i] / (double)sum);
  }

  assert (!rc);
  return 0;
}


//what the test is: 2 instances of reader_thread and 2 instances of writer_thread
// so only 2 readers can be active at a time
// BUT, those readers will keep reading forever
