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

void create_request(Request *req) {
    req->method = NULL;
    req->uri = NULL;
    req->version = NULL;
    req->original_pointer = NULL;
    req->msg = NULL;
    req->rwlock_ptr = NULL;
    req->len_msg = 0;
    req->fd = 0;
    req->status_code = 0;
    req->request_id = 0;
}

void free_request(Request *req) {
    //printf("In free request\n");
    if (req != NULL) {
        free(req->method);
        free(req->uri);
        free(req->version);
        free(req->rwlock_ptr);
        free(req->original_pointer);
        // we do not free req->msg bc it points to memory allocated by original_pointer
    }
}

int my_read_n_bytes(int fd, char buffer[], int n) {
    // struct timeval tv, tv2;
    // gettimeofday(&tv, NULL);
    // unsigned long long milliseconds = (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
    // printf("\nmy_read_n_bytes() started Milliseconds since epoch: %llu\n", milliseconds);
    int bytes_read = 0;
    char *p = buffer;

    struct timeval timeout; // Structure for timeout value
    timeout.tv_sec = 1; // Set timeout to 5 seconds (adjust as needed)
    timeout.tv_usec = 0; //.05 seconds, not sure if this is enough

    while (bytes_read < n) {
        fd_set rfds; // File descriptor set for select()
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        int ret = select(fd + 1, &rfds, NULL, NULL, &timeout);
        if (ret == -1) {
            perror("select");
            return -1;
        } else if (ret == 0) {
            printf("\ntimeout in select()");
            fflush(stdout);
            // Handle timeout (e.g., retry or break)
            break;
        }

        int just_read = read(fd, p, n - bytes_read);
        //printf("\n just_read is %d", just_read);
        fflush(stdout);
        if (just_read == -1) {
            int read_error = errno;
            if (read_error != EAGAIN) { // Not a temporary error (e.g., EWOULDBLOCK)
                perror("read");
                printf("\nactual error in read()");
                fflush(stdout);
                return -1;
            } else {
                printf("\njust a timeout, we continuing in read()");
                fflush(stdout);
            }
        } else if (just_read == 0) {
            break; // EOF (end of file)
        }

        bytes_read += just_read;
        p += just_read;
    }

    // gettimeofday(&tv2, NULL);
    // unsigned long long milliseconds2 = (unsigned long long)(tv2.tv_sec) * 1000 + (unsigned long long)(tv2.tv_usec) / 1000;
    // printf("\nmy_read_bytes() end: %llu\n", milliseconds2);
    // fflush(stdout);
    return bytes_read;
}

//errno will update only if funct failed
//if it succeeded, it will have the last value of failed funct
//mem is memory that must be freed if we are calling response
//I'm using -10 as default no file descriptor value
int handle_io_error(Request *request, int ret, int err, void *mem, int fd) {
    // struct timeval tv, tv2;
    // gettimeofday(&tv, NULL);
    // unsigned long long milliseconds = (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
    // printf("\nhandle_io_error() start: %llu\n", milliseconds);
    if (ret == -1 && err != 11) {
        //not a timeout, actual issue
        printf("\nhandle_io_error() : issue not timeout, calling response 500");
        if (mem != NULL)
            free(mem);
        if (fd != -10)
            close(fd);
        return response(request, 500); //internal server error
    } else if (ret == -1)
        printf("\nhandle_io_error() : timeout, continuing");
    // gettimeofday(&tv2, NULL);
    // unsigned long long milliseconds2 = (unsigned long long)(tv2.tv_sec) * 1000 + (unsigned long long)(tv2.tv_usec) / 1000;
    // printf("\nhandle_io_error() end: %llu\n", milliseconds2);

    fflush(stdout);
    return 0;
}

// ex: GET,/b.txt,404,2
void audit_log(Request *request) {
    //add check for if request->status_code != 500?
    if (request->method != NULL && request->uri != NULL) {
        fprintf(stderr, "%s,/%s,%d,%d\n", request->method, request->uri, request->status_code,
            request->request_id);
        printf("\nprinted audit log");
    } else
        printf("\ndid not print audit log");
}

//outputs response to server of appropriate status-code using a switch statement
int response(Request *request, int status_code) {
    //printf("\n Request called with status code: %d\n", status_code);
    int write_result = 0;
    char *msg;
    request->status_code = status_code;
    //fprintf(stderr, "\nWriting to stderr");
    audit_log(request);

    switch (status_code) {
    case 200: //OK
        msg = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        break;
    case 201: //file created in put
        msg = "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        break;
    case 400: //bad request
        msg = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        break;
    case 403: //uri's file is not accessible
        msg = "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        break;
    case 404: //uri's file dne
        msg = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        break;
    case 500: //unexpected issue: internal server error
        msg = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server "
              "Error\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        break;
    case 501: //request has an unimplemented method
        msg = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 16\r\n\r\nNot Implemented\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        break;
    case 505: //request has an unsupported version
        msg = "HTTP/1.1 505 Version Not Supported\r\nContent-Length: 22\r\n\r\nVersion Not "
              "Supported\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        break;
    }
    if (write_result == -1)
        ; //call 500?

    fflush(stdout);
    return 0;
}

int list_lookup(Request *request) {
    pthread_mutex_lock(&List_Lock); //List access mutex lock
    bool in_list = false;
    for (moveFront(L); length(L) > 0 && cursor(L) >= 0; moveNext(L)) {
        char *g = (char *) get1(L);
        printf("\n g is %s", (char *) g);
        if (strcmp(g, request->uri) == 0) {
            in_list = true;
            break;
        }
    }
    if (!in_list) { //uri not in List
        printf("\nNot in List, thr_id: %d", request->thr_id);
        request->rwlock_ptr = rwlock_new(2, 2); //N-WAY with 2 reads between each write
        prepend(L, request->uri, request->rwlock_ptr);
        moveFront(L); //not needed for my testing
        printf("\nrequest.uri is %s, data1 is %s, thr_id: %d", request->uri, (char *) get1(L),
            request->thr_id);
    } else {
        printf("\nin List, thr_id: %d", request->thr_id);
        request->rwlock_ptr = (rwlock_t *) get2(L);
    }
    printf("\nthe uri is %s. the rwlock is stored at %p, thr_id: %d", (char *) get1(L), get2(L),
        request->thr_id);
    pthread_mutex_unlock(&List_Lock); //List access mutex unlock
    return 0;
}

// return strlen of matched expression or -1 for failure
// parameter type: 0 = request line, or 1 = header field
// returns -1 if there was not a match and 0 if there was an issue with regex itself
// returns number of bytes that were parsed if it was a successful parse

//TODO: add request-line to header-fields!
int parsing_function(
    Request *request, const char *request_string, const char *request_expr, int type) {
    regex_t re_object;
    int result = regcomp(&re_object, request_expr, REG_EXTENDED | REG_NEWLINE);

    if (result) { //if reuslt is 0, successful
        char buffer[100];
        regerror(result, &re_object, buffer, 100);
        printf("\nregcomp() failed with '%s'\n", buffer);
        return result;
    }

    regmatch_t pmatch[re_object.re_nsub + 1]; //make list of possible pmatch + 1
    result = regexec(&re_object, request_string, re_object.re_nsub + 1, pmatch, 0);

    if (result == 1) //It is not a match
        return -1;

    if (result) { //if reuslt is 0, successful
        char buffer[100];
        regerror(result, &re_object, buffer, 100);
        printf("regexec() failed with '%s'\n", buffer);
        return result;
    }

    //now we have each element in pmatch as
    //typedef struct {
    //   regoff_t  rm_so;     //match start index, -1 if no match
    //   regoff_t  rm_eo;	  //match end index
    //} regmatch_t;

    int count = 0;
    char *key = NULL;

    for (unsigned long i = 1; i < re_object.re_nsub + 1; i++) {
        int length = pmatch[i].rm_eo - pmatch[i].rm_so;
        count += length;
        //does it matter that i am not null terminating any of these?
        if (type == 0) { //Request Line
            if (i == 1) { //Method
                request->method = (char *) calloc(length + 1, sizeof(char));
                memcpy(request->method, request_string + pmatch[i].rm_so, length);
            } else if (i == 2) { //URI
                request->uri = (char *) calloc(length + 1, sizeof(char));
                memcpy(request->uri, request_string + pmatch[i].rm_so, length);
                list_lookup(request);
            } else if (i == 3) { //Versioin
                request->version = (char *) calloc(length + 1, sizeof(char));
                memcpy(request->version, request_string + pmatch[i].rm_so, length);
            }
        } else if (type == 1) { //Header Field
            if (i == 1) { //Key
                key = (char *) calloc(length + 1, sizeof(char));
                memcpy(key, request_string + pmatch[i].rm_so, length);
                printf("\nKey for header field is %s, thr_id : %d", key, request->thr_id);
            } else if (i == 2) { //Value
                if (key != NULL && strcmp(key, "Content-Length") == 0) {
                    char *value = (char *) calloc(length + 1, sizeof(char));
                    memcpy(value, request_string + pmatch[i].rm_so, length);
                    //convert value to an int
                    for (int i = 0; i < length; i++) {
                        if (isdigit(value[i]) == 0) {
                            printf("\ncalling response from regex");
                            response(request, 400); //Invalid Command
                        }
                    }
                    request->len_msg = atoi(value);
                    free(value);
                    value = NULL;
                } else if (key != NULL && strcmp(key, "Request-Id") == 0) {
                    char *value = (char *) calloc(length + 1, sizeof(char));
                    memcpy(value, request_string + pmatch[i].rm_so, length);
                    printf("\nRequest-ID is %s for thr_id : %d", value, request->thr_id);
                    //convert value to an int
                    for (int i = 0; i < length; i++) {
                        if (isdigit(value[i]) == 0) {
                            printf("\ncalling response from regex");
                            response(request, 400); //Invalid Command
                        }
                    }
                    request->request_id = atoi(value);
                    free(value);
                    value = NULL;
                }
                free(key);
                key = NULL;
            }
        }
    }
    regfree(&re_object); //FREE ALL MY BITS
    return count;
}
