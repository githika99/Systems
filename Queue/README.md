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

## Makefile: 
creates the queue executable 

## Use:
make all - produces the queue binary<br>
make queue - produces the queue binary<br>
make checkMemoryQueue - runs valgrind on ./queue<br>
make clean - removes all .o and binary files<br>
make format - clang-formats queue.c and rwlock.c<br>

### Author: Githika Annapureddy
### Date: 2/29/2024

