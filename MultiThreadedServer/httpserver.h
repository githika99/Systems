#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "my_helpers.h" //for the Request struct
#include "List.h" //to define List L;
#include "pthread.h" //to define pthread_mutex_t List_Lock;

extern pthread_mutex_t List_Lock;
extern List L;

void *thread_function(void *args);

void *dispatcher(void *args);

int getopt_parser(int argc, char **argv);

void handle_shutdown(int sig);

int main(int argc, char **argv);

#endif
