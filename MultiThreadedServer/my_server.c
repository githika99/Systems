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

int put(Request *request) {
    writer_lock(request->rwlock_ptr); //call writer_lock for List[uri]

    printf("\nPut Entered, thr_id: %d", request->thr_id);
    fflush(stdout);
    bool file_exists = true;
    if (access(request->uri, F_OK) != 0) //if file dne
        file_exists = false;
    //commenting out bc getting errors, do not know if this is source of errors
    // struct stat file_stat; //more file checks
    // if (stat(request->uri, &file_stat) == -1){  //file has bad permissions was handled by process, do this is only file DNE
    //     file_exists = false;
    // }
    int fd = open(request->uri, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd == -1) {
        writer_unlock(request->rwlock_ptr); //release writer_lock for List[uri]
        return response(request, 500); //error reading file
    }

    //write msg into file
    int write_result = write_n_bytes(fd, request->msg, request->len_msg);
    handle_io_error(request, write_result, errno, NULL, fd);

    close(fd);
    fflush(stdout);
    writer_unlock(request->rwlock_ptr); //release writer_lock for List[uri]
    if (file_exists)
        return response(request, 200); //if file exists

    return response(request, 201); //if you made file
}

int get(Request *request) {
    printf("\nEnetered get, thr_id: %d", request->thr_id);
    printf("\nwaiting to aquire readerlock for %p, thr_id: %d", (void *) request->rwlock_ptr,
        request->thr_id); fflush(stdout);
    reader_lock(request->rwlock_ptr); //call reader_lock for List[uri]
    printf("\ngot readerlock for %p, thr_id: %d", (void *) request->rwlock_ptr, request->thr_id);
    fflush(stdout);

    int fd = open(request->uri, O_RDONLY, 0000);
    if (fd == -1) {
        printf("\n%s could not open, calling 403 error", request->uri);
        close(fd);
        return response(request, 403);
    }

    //getting size of file: uri
    struct stat file_status;
    if (stat(request->uri, &file_status) < 0) { //did not work
        close(fd);
        printf("\nreleasing readerlock for %p, thr_id: %d", (void *) request->rwlock_ptr,
            request->thr_id);
        reader_unlock(request->rwlock_ptr); //release reader_lock for List[uri]
        return response(request, 500);
    }

    int file_size = file_status.st_size;
    int msg_length = snprintf(NULL, 0, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", file_size);
    char *msg = malloc(msg_length + 1);
    sprintf(msg, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", file_size);

    int write_result = write_n_bytes(request->fd, msg, strlen(msg));
    free(msg);
    msg = NULL;
    handle_io_error(request, write_result, errno, NULL, fd);

    int curr_size = SIZE2;
    char *buffer = calloc(SIZE2, sizeof(char));
    int length = 0;
    int read_result;
    do {
        if (length != 0) {
            curr_size += SIZE2;
            buffer = (char *) realloc(buffer, curr_size);
        }
        read_result = my_read_n_bytes(fd, buffer, SIZE2);
        handle_io_error(request, read_result, errno, NULL, fd);
        length += read_result;

    } while (read_result > 0);
    printf("\nlength of read %d, thr_id: %d", length, request->thr_id);

    write_result = write_n_bytes(request->fd, buffer, length);
    printf("\nwrite_result is %d, thr_id: %d", write_result, request->thr_id);
    handle_io_error(request, write_result, errno, NULL, fd);
    fflush(stdout);
    close(fd);

    request->status_code = 200;
    audit_log(request);

    printf(
        "\nreleasing readerlock for %p, thr_id: %d", (void *) request->rwlock_ptr, request->thr_id);
    reader_unlock(request->rwlock_ptr); //release reader_lock for List[uri]

    //didn't call response so handle ending yourself
    free_request(request);
    request = NULL;

    fflush(stdout);
    return 0;
}

int process(int socket_fd, int thr_id) {

    //use to be in main()
    Request request;
    create_request(&request);
    request.fd = socket_fd;
    request.thr_id = thr_id;

    printf("\nEntered process(), thr_id: %d", thr_id);

    //testing to see if it will break with this num as SIZE
    int curr_size = SIZE2;
    request.original_pointer = (char *) calloc(SIZE2, sizeof(char));
    int length_2 = 0;
    int just_read;
    do {
        if (length_2 != 0) {
            curr_size += SIZE2;
            request.original_pointer = (char *) realloc(request.original_pointer, curr_size);
        }
        just_read = my_read_n_bytes(request.fd, request.original_pointer + length_2, SIZE2);
        handle_io_error(&request, just_read, errno, NULL, -10);
        length_2 += just_read;
    } while (just_read > 0);

    //making a smaller copy of first 4069 bytes to pass to parsing function, takes a long time if you pass whole string
    char *string = calloc(SIZE + 1, sizeof(char));
    memcpy(string, request.original_pointer, SIZE);
    string[SIZE] = '\0'; //null terminate

    printf("\nlength_2 = %d, thr_id: %d", length_2, thr_id);

    //Parsing Request Line
    printf("\nprocessing request, thr_id: %d", thr_id);
    fflush(stdout);
    const char *request_expr = "^([a-zA-Z]{0,8}) /([a-zA-Z0-9.-]{2,64}) (HTTP/[0-9].[0-9])\r\n";
    int bytes = parsing_function(&request, string, request_expr, 0);
    if (bytes == -1) {
        printf("\nRequest Line match failed, thr_id: %d", thr_id);
        fflush(stdout);
        free(string);
        return response(&request, 400); //BAD REQUEST
    } else if (bytes == 0) {
        free(string);
        return response(&request, 500); //internal server error
    }
    bytes += 5; //for 2 spaces + '\' in front of uri + '\r\n'

    //Parsing Each Header File
    int num = 0;
    request_expr = "^([a-zA-Z0-9.-]{1,128}): ([ -~]{0,128})\r\n";
    int curr = parsing_function(&request, string + bytes, request_expr, 1);
    while (curr != -1) {
        if (curr == 0) {
            free(string);
            printf("\nparsing function returned 0, calling 500 error, thr_id: %d", thr_id);
            return response(&request, 500); //internal server error
        }
        bytes += curr + 4; //for 1 colon + space + '\r\n'
        num++;
        curr = parsing_function(&request, string + bytes, request_expr, 1);
    }

    printf("\nexited parsing_function, thr_id: %d", thr_id);
    fflush(stdout);

    // Check that next two chars are '\r\n'
    if ((string[bytes] != '\r' && string[bytes] != '\n')
        | (string[bytes + 1] != '\n' && string[bytes + 1] != '\r')) {
        printf("\nDoes not have extra \\r\\n\\r\\n, thr_id: %d", thr_id);
        free(string);
        return response(&request, 400);
    }
    bytes += 2; //for '\r\n'

    printf("\nRequest is '%.*s'\n, thr_id: %d", bytes, string, thr_id);

    free(string);

    //trim first bytes bytes of request.msg
    request.msg = request.original_pointer + bytes;

    printf("request.method is now: %s\n, thr_id: %d", request.method, thr_id);
    printf("request.uri is now: %s\n, thr_id: %d", request.uri, thr_id);
    printf("request.version is now: %s\n, thr_id: %d", request.version, thr_id);
    printf("request.len_msg is now: %d\n, thr_id: %d", request.len_msg, thr_id);
    printf("length of request.original_pointer is: %d\n, thr_id: %d", length_2, thr_id);

    printf("\nCalling get() put() or error(), thr_id: %d", thr_id);

    if (strcmp(request.method, "GET") != 0
        && strcmp(request.method, "PUT") != 0) //check if method is not GET nor PUT
        return response(&request, 501);

    if (strcmp(request.version, "HTTP/1.1") != 0)
        return response(&request, 505);

    if (length_2 != bytes + request.len_msg) { //Content-Length does not match
        printf("\nContent-length and total not same size, Content-length is %d, length_2 - bytes "
               "is %d",
            request.len_msg, length_2 - bytes);
        return response(&request, 400);
    }

    struct stat file_stat; //more file checks
    if (strcmp(request.method, "GET") == 0) {
        printf("\nentered get case, thr_id: %d", thr_id);
        if (request.len_msg != 0)
            return response(&request, 400); //bad response
        else if (access(request.uri, F_OK) != 0) //file dne
            return response(&request, 404);
        else if (stat(request.uri, &file_stat) == -1) { //extra checks
            if (errno == EACCES) //bad permissions
                return response(&request, 403);
            else //file dne
                return response(&request, 404);
        } else if (stat(request.uri, &file_stat) == 0
                   && S_ISDIR(file_stat.st_mode)) { //checks if it is a directory
            printf("%s is a directory, calling 403 error", request.uri);
            return response(&request, 403);
        } else if (access(request.uri, R_OK) != 0) { //read permissions not enabled
            printf("%s is a directory, calling 403 error", request.uri);
            return response(&request, 403);
        } else
            return get(&request);
    } else {
        printf("\nentered Put case in process, thr_id: %d", thr_id);
        printf("\naccess(request.uri, F_OK) == %d", access(request.uri, F_OK));
        if (access(request.uri, F_OK) == 0
            && access(request.uri, W_OK & R_OK)
                   != 0) { //file exists but does not have write permissions
            printf("\nfirst put case");
            return response(&request, 403);
        }
        // else if (stat(request.uri, &file_stat) == -1 && errno == EACCES) { //bad permissions
        //     printf("\nfirst put case");
        //     return response(&request, 403);
        // }
        else {
            printf("\ncalling put(), thr_id: %d", thr_id);
            put(&request);
        }
    }
    fflush(stdout);
    return 0;
}
