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
void set_done(int i) {
  pthread_mutex_lock(&dmutex);
  done = i;
  pthread_mutex_unlock(&dmutex);
}

int push_num = 0;

void* push_thread(void *args) {
    uintptr_t id = (uintptr_t)args;

    while (!get_done()) {
        //random_sleep();
        int * m = malloc(sizeof(int));
        if (m == NULL){
            printf("\nMalloc failed");
            exit(1);
        }
        *m = push_num;  //copies the value of push_num into the place pointed to by m
        push_num++;
       
        queue_push(queue_d, m);
        printf("\nqueue %lu PUSHED: %d\n", id, *m);
        counts[id]++;
    }

    printf("\npush_thread %lu exiting\n", id);
    return NULL;
}

void* pop_thread(void *args) {
    uintptr_t id = (uintptr_t)args;
    printf("\npop_thread %lu entering\n", id);

    while (!get_done()) {
        //random_sleep();
        void* elem;
        counts[id]++;
        if (queue_pop(queue_d, &elem)) {
            if (elem != NULL) { 
                printf("\nqueue %lu POPPED: %d seconds\n", id, *(int*)elem); // Use %ld to print time_t
                free(elem);
            }
        }
    }
    printf("\npop_thread %lu exiting\n", id);
    return NULL;
}


int call(void) {
  pthread_t threads[THREADS];
  int rc = 0;
  uintptr_t sum = 0;

  //queue_d = queue_new(10);
  pthread_create(threads , NULL, push_thread, (void*)0);
  pthread_create(threads + 1, NULL, push_thread, (void*)1);
  pthread_create(threads + 2, NULL, pop_thread, (void*)2);
  pthread_create(threads + 3, NULL, pop_thread, (void*)3);

  // sleep for some time. then cancel all of the threads
  sleep(2);
  set_done(1);


  for (int i = 0; i < THREADS; ++i) {
    pthread_join(threads[i], NULL);
    sum += counts[i];
  }


  //queue_delete(&queue_d);

  for (int i = 0; i < THREADS; ++i) {
    fprintf(stderr, "%i: %i (%lf)\n", i, counts[i], (double)counts[i] / (double)sum);
  }

  assert (!rc);
  return 0;
}


int main(void){

    queue_d = queue_new(10);
    call();
    set_done(0); //reset done so that 4th pop thread has a chance to go
    printf("\nDone is now: %d", get_done());
    pthread_t tid1;
    //call pop_thread one last time to make sure last pushes are popped
    pthread_create(&tid1, NULL, pop_thread, (void*)4);
    sleep(1);
    set_done(1);
    printf("\nDone is now: %d", get_done());
    if (pthread_join(tid1, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }
    queue_delete(&queue_d);
    return 0;
}


//done is 0 to go
//done is 1 to stop 


//pop doesn't exit because it is supposed to wait forever for another push. 
//this is expected behavior, so its fine. 
