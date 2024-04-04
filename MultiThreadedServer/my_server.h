#ifndef MY_SERVER_H
#define MY_SERVER_H

#include "my_helpers.h" //for the Request struct

int put(Request *request);

int get(Request *request);

int process(int socket_fd, int thr_id);

#endif
