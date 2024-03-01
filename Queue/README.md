## queue.c: 
implements a FIFO queue with locks 

### functions in queue.c
queue_t *queue_new(int size); - creates, initializes, and returns a new queue_t object that can hold as most size objects. <br>

void queue_delete(queue_t **q); - frees any memory allocated for the queue_t q, and sets all pointers to NULL. <br>

bool queue_push(queue_t *q, void *elem); - Pushes the item elem onto the queue_t q. Push is blocked if the queue is full (number of elements equals size). Returns false if the queue_t object queue is NULL, else returns true.<br>

bool queue_pop(queue_t *q, void **elem); - Pops an item from the queue_t q. Pop is blocked if the queue is empty (number of elements equals 0). Returns false if the queue_t object queue is NULL, else returns true.<br>

### data structures in queue.c
I create a queue struct: typedef struct queue queue_t;. The queue can hold any datatype. The struct has the following fields int in, out, size; void **buffer, and sem_t empty_cells, full_cells, mutex. <br>

The queue does not free the memory in the buffer, but it frees the actual buffer in queue_delete(). 

## rwlock.c: 

### functions in rwlock.c
rwlock_t* = rwlock_new(PRIORITY p, int n); - creates, initializes, and returns a new rwlock_t pointer object that is initialized with PRIORITY p and N-WAY n. n is ignored when p is is READERS or WRITERS<br>

void rwlock_delete(rwlock_t **l); - frees any memory allocated for the rwlock_t l, and sets all pointers to NULL. <br>

void reader_lock(rwlock_t *rw); - locks the shared resource for reading. Allows multiple readers to access the critical section simultaneously, but no writers. <br>

void reader_unlock(rwlock _t *rw); - unlocks the shared resource for reading. A writer is given a chance to acquire the lock.<br>

void writer_lock(rwlock_t *rw); - locks the shared resource for writing. Allows a single writer to access the critical section at time, but no other writers or reades. <br>

void writer_unlock(rwlock_t *rw); - unlocks the shared resource for writing. Allows a single or group of readers to take control of the lock. <br>


### data structures in rwlock.c
I create a rwlock struct : typedef struct rwlock rwlock_t;. The fields in the struct are PRIORITY, int n, int waiting_readers, waiting_writers, active_readers, active_writers, signal, n_readers, pthread_mutex_t writer_lock, lock; pthread_cond_t reader, writer.<br>

I used a user-defined type called PRIORITY that is of type enum: typedef enum {READERS, WRITERS, N_WAY} PRIORITY;. If PRIORITY is set to READERS, it should give priority to calls to read_lock() over write_lock() when there is contention. If it is set to WRITERS, it should give priority to calls to write_lock(). If it is set to N-WAY, it should allow n readers to proceed between every write.<br>

## Makefile: 
creates the queue and rwlock executables 

## Use:
make all - produces the queue and rwlock executables <br>
make queue - produces the queue binary<br>
make rwlock - produces the rwlock binary<br>
make clean - removes all .o and binary files<br>
make format - clang-formats queue.c and rwlock.c<br>

### Author: Githika Annapureddy
### Assignment: Asgn 3
### Date: 2/29/2024

