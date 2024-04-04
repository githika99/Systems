#ifndef MY_HELPERS_H
#define MY_HELPERS_H

#include "rwlock.h" //to decalre rwlock_t * rwlock_ptr;

//This Struct exists to store different elements of the Request
typedef struct {
    int fd;
    int status_code;
    int request_id;
    char *method;
    char *uri;
    char *version;
    char *original_pointer;
    char *msg;
    int len_msg;
    int thr_id;
    rwlock_t *rwlock_ptr;
} Request;

void create_request(Request *req);

void free_request(Request *req);

int my_read_n_bytes(int fd, char buffer[], int n);

int handle_io_error(Request *request, int ret, int err, void *mem, int fd);

void audit_log(Request *request);

int response(Request *request, int status_code);

int list_lookup(Request *request);

int parsing_function(
    Request *request, const char *request_string, const char *request_expr, int type);

#endif
