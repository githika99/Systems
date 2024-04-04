/*
Githika Annapureddy
CruzId: gannapur
Assignment: pa4
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include "asgn2_helper_funcs.h"
#include "httpserver.h"
#include "my_helpers.h"
#include "my_server.h"
#include "queue.h"
#include "rwlock.h"
#include "List.h"
#define SIZE  2049
#define SIZE2 1000000

//put locks around shared variables, like files

extern int THREADS;
int THREADS = 4;
extern queue_t *queue_d;
int port;
queue_t *queue_d = NULL;
int while_loop_started = 0;

int dis_id = 0;
int thr_id = 0;

pthread_mutex_t List_Lock;
List L = NULL;

void *thread_function(void *args) {
    (void) args;
    while (1) {
        void *elem;
        printf("\ntrying to pop from queue, thr_id: %d", thr_id);
        fflush(stdout);
        if (queue_pop(queue_d, &elem)) {
            if (elem != NULL) {
                thr_id++;
                printf("\nsuccessfully popped %d from queue, thr_id: %d", *(int *) elem,
                    thr_id); // Use %ld to print time_t
                process(*(int *) elem, thr_id); //typecast to int first
                close(*(int *) elem);
            }
        }
    }
    return NULL;
}

//only one dispatcher thread at a time
void *dispatcher(void *args) {
    (void) args;
    Listener_Socket sock;

    int result = listener_init(&sock, port);
    if (result == -1) {
        printf("Could not intialize socket for port # %d", port);
        queue_delete(&queue_d);
        exit(1);
    }
    printf("\n Listener socket initialized");
    fflush(stdout);

    pthread_mutex_init(&List_Lock, NULL);
    L = newList(); //initialize List
    prepend(L, "badtestfile", "badtestlock");

    while (1) {
        printf("\nwhile loop entered, dis_id: %d", dis_id);
        //while_loop_started = 1;
        printf("\nlistening for socket, dis_id: %d", dis_id);
        int fd = listener_accept(&sock);
        printf("\nfinished for socket, dis_id: %d", dis_id);
        if (fd == -1) {
            printf("Could not accept socket, dis_id: %d", dis_id);
            queue_delete(&queue_d);
            exit(1);
        }
        printf("\n Listener socket accepted: connection established, dis_id: %d", dis_id);
        fflush(stdout);
        int *m = malloc(sizeof(int));
        if (m == NULL) {
            printf("\nMalloc failed, exiting, dis_id: %d", dis_id);
            queue_delete(&queue_d);
            exit(1);
        }

        *m = fd; //copies the value of fd into the place pointed to by m

        printf("\ntrying to push to queue, dis_id: %d", dis_id);
        fflush(stdout);
        queue_push(queue_d, m);
        printf("\nsuccessfully pushed %d to queue, dis_id: %d", *m, dis_id);
        fflush(stdout);
        dis_id++;
    }
}

extern char *optarg;
extern int optind, opterr, optopt;
int getopt_parser(int argc, char **argv) {
    opterr = 0; //no error msgs are printed to stderr
    int opt;
    while ((opt = getopt(argc, argv, ":t:")) != -1) {
        if (opt == 't') {
            printf("for %c, optarg is :%s\n", opt, optarg);
            THREADS = atoi(optarg);
            if (THREADS == 0) { //optarg is contains non-digits
                printf("-t __ more than just digits. returning -1\n");
                return -1; //how to handle this error
            }
        } else if (opt == '?') {
            printf("unknown option: %c. returning -1\n", optopt);
            return -1; //how to handle this error
        }
    }

    //check that argv[optind] is last variable in argv
    if (argv[optind] == NULL) {
        printf("port number not specified after the -t flag\n");
        return -1; //how to handle this error
    }
    if (argv[optind + 1] != NULL) {
        printf("extra information after port number\n");
        return -1; //how to handle this error
    }
    printf("remaining = %s", argv[optind]);

    port = atoi(argv[optind]);
    if (port == 0) {
        printf("Port number not just digits\n");
        return 1;
    }
    if (port < 1 || port > 65535) {
        printf("Invalid port number");
        return 1;
    }

    return 0;
}

void handle_shutdown(int sig) {
    printf("Received signal %d, shutting down...\n", sig);
    if (while_loop_started) {
        queue_delete(&queue_d); //free queue, will handle if queue is NULL
        printf("\ndeleted queue");
        fflush(stdout);
        //delete semaphores here (not strictly necessary)
    }
    if (sig == SIGINT)
        raise(SIGINT);
    else if (sig == SIGTERM)
        raise(SIGTERM);
}

//sets up server, calls buffer_processing function, frees request,
int main(int argc, char **argv) {
    printf("\n Entered main function of http server");

    if (getopt_parser(argc, argv) < 0)
        return -1;

    printf("\nTHREADS is %d", THREADS);
    fflush(stdout);
    printf("\nport number is %d", port);
    fflush(stdout);

    queue_d = queue_new(THREADS);
    //define threads
    pthread_t all_threads[THREADS + 1];

    //create all threads
    for (int i = 0; i < THREADS + 1; ++i) {
        if (i == THREADS)
            pthread_create(all_threads + i, NULL, dispatcher, NULL);
        else
            pthread_create(all_threads + i, NULL, thread_function, NULL);
    }

    //call pthread_join for all threads
    for (int i = 0; i < THREADS + 1; ++i) {
        pthread_join(all_threads[i], NULL);
    }

    // signal(SIGINT, handle_shutdown); // associates handle_shutdown with SIGINT -  Ctrl+C
    // signal(SIGTERM, handle_shutdown); // associates handle_shutdown with SIGTERM -  termination

    printf(
        "\nall threads done, continuing with queue_d"); //will never happen since threads run forever
    queue_delete(&queue_d);
    pthread_mutex_destroy(&(List_Lock));

    printf("\nfinished all deletes/frees");
    fflush(stdout);
    return 0;
}
