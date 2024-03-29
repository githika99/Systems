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
creates the rwlock executable

## Use:
make all - produces the rwlock executable <br>
make rwlock - produces the rwlock binary<br>
make checkMemoryRWLock - runs valgrind on ./rwlock<br>
make clean - removes all .o and binary files<br>
make format - clang-formats queue.c and rwlock.c<br>

### Author: Githika Annapureddy
### Date: 2/29/2024

