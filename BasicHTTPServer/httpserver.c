/*
Githika Annapureddy
CruzId: gannapur
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h> //includes PATH_MAX
#include <ctype.h>
#include <unistd.h>
#include <regex.h>
#include <sys/stat.h>
#include <time.h>
#include "asgn2_helper_funcs.h"
#define SIZE  2049
#define SIZE2 1000000

//This Struct exists to store different elements of the Request
typedef struct {
    int fd;
    char *method;
    char *uri;
    char *version;
    int len_msg;
    char *msg;
} Request;

void create_request(Request *req) {
    req->method = NULL;
    req->uri = NULL;
    req->version = NULL;
    req->msg = NULL;
    req->len_msg = 0;
    req->fd = 0;
}

void free_request(Request *req) {
    if (req != NULL) {
        free(req->method);
        free(req->uri);
        free(req->version);
        free(req->msg);
    }
}

int my_read_n_bytes(int fd, char buffer[], int n) {
    int bytes_read = 0;
    char *p = buffer;
    int just_read = 0;
    while (bytes_read < n) {
        just_read = read(fd, p, n - bytes_read); //read remaining num of bytes
        if (just_read == -1) { //use errno to make sure it is not a timeout,
            int read_error = errno; //if it is a timeout return # of bytes read
            if (read_error != 11) //errno of 11 means timeout
                return -1;
            else
                break;
        } else if (just_read == 0)
            break; //done reading
        bytes_read += just_read;
        p += just_read;
    }
    return bytes_read;
}

//outputs response to server of appropriate status-code using a switch statement
int response(Request *request, int status_code) {
    int write_result;
    char *msg;

    switch (status_code) {
    case 200: //OK
        msg = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        if (write_result == -1)
            ; //call 500?
        break;
    case 201: //file created in put
        msg = "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        if (write_result == -1)
            ; //call 500?
        break;
    case 400: //bad request
        msg = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        if (write_result == -1)
            ; //call 500?
        break;
    case 403: //uri's file is not accessible
        msg = "HTTP/1.1 403 Forbidden\r\nContent-Length: 10\r\n\r\nForbidden\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        if (write_result == -1)
            ; //call 500?
        break;
    case 404: //uri's file dne
        msg = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        if (write_result == -1)
            ; //call 500?
        break;
    case 500: //unexpected issue: internal server error
        msg = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 22\r\n\r\nInternal Server "
              "Error\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        if (write_result == -1)
            ; //call 500?
        break;
    case 501: //request has an unimplemented method
        msg = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 16\r\n\r\nNot Implemented\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        if (write_result == -1)
            ; //call 500?
        break;
    case 505: //request has an unsupported version
        msg = "HTTP/1.1 505 Version Not Supported\r\nContent-Length: 22\r\n\r\nVersion Not "
              "Supported\n";
        printf("\nSupposed to write %s to server", msg);
        write_result = write_n_bytes(request->fd, msg, strlen(msg));
        if (write_result == -1)
            ; //call 500?
        break;
    }

    return 0;
}

int put(Request *request, int length) {
    bool file_exists = true;
    if (access(request->uri, F_OK) != 0) //if file dne
        file_exists = false;
    int fd = open(request->uri, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd == -1)
        return response(request, 500); //error reading file

    //1. write all of *msg to file
    int write_result = write_n_bytes(fd, request->msg, length);
    if (write_result == -1) {
        printf("\nwrite= -1, calling response(500)");
        response(request, 500);
    }

    //2. read rest of input here, and write in a loop -> do not store read and write from buffer then clear like in asgn1
    int total = length;
    int read_result = 0;
    write_result = 0;
    do { //works but buffer is not rewritten each time (contains last input,
        char buffer
            [SIZE2]; //but it doens't matter this time since we're not doing anything with the buffer)
        read_result = my_read_n_bytes(request->fd, buffer, SIZE2); //read from file
        total += read_result;
        if (read_result == -1) {
            printf("\nread= -1, calling response(500)");
            response(request, 500);
        }

        write_result = write_n_bytes(fd, buffer, read_result);
        if (write_result == -1) {
            printf("\nwrite= -1, calling response(500)");
            response(request, 500);
        }
        length += write_result;
    } while (read_result > 0);

    if (request->len_msg != total) {
        //Content-Length does not match
        printf("Content-length and write_result not same size");
        return response(request, 400);
    }

    close(fd);
    if (file_exists)
        return response(request, 200); //if file exists
    return response(request, 201); //if you made file
}

int get(Request *request) {
    int fd = open(request->uri, O_RDONLY, 0000);
    if (fd == -1) {
        printf("%s could not open, calling 403 error", request->uri);
        perror("open");
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        return response(request, 403); //incorect permissions
    }
    int read_result = 0;
    int file_size = 0;

    //getting size of file: uri
    struct stat file_status;
    if (stat(request->uri, &file_status) < 0) //did not work
        file_size = -1;
    file_size = file_status.st_size;
    int msg_length = snprintf(NULL, 0, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", file_size);
    char *msg = malloc(msg_length + 1);
    sprintf(msg, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", file_size);

    int write_result = write_n_bytes(request->fd, msg, strlen(msg));
    if (write_result == -1) {
        free(msg);
        msg = NULL;
        response(request, 500);
    }

    do { //works but buffer is not rewritten each time (contains last input,
        char buffer
            [SIZE2]; //but it doens't matter this time since we're not doing anything with the buffer)
        read_result = my_read_n_bytes(fd, buffer, SIZE2); //read from file
        if (read_result == -1) {
            free(msg);
            msg = NULL;
            printf("\nread= -1, calling response(500)");
            response(request, 500);
        }
        int write_result = write_n_bytes(request->fd, buffer, read_result);
        if (write_result == -1) {
            free(msg);
            msg = NULL;
            printf("\nwrite= -1, calling response(500)");
            response(request, 500);
        }
    } while (read_result > 0);

    close(fd);
    free(msg);
    msg = NULL;
    return 0;
}

// return strlen of matched expression or -1 for failure
// parameter type: 0 = request line, or 1 = header field
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
            } else if (i == 3) { //Versioin
                request->version = (char *) calloc(length + 1, sizeof(char));
                memcpy(request->version, request_string + pmatch[i].rm_so, length);
            }
        } else if (type == 1) { //Header Field
            if (i == 1) { //Key
                key = (char *) calloc(length + 1, sizeof(char));
                memcpy(key, request_string + pmatch[i].rm_so, length);
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
                }
                free(key);
                key = NULL;
            }
        }
    }
    regfree(&re_object); //FREE ALL MY BITS
    return count;
}

int process(Request *request, char *string, int length) {
    int curr;
    //const char *request_expr = "^([a-zA-Z]{0,8}) ([a-zA-Z0-9.-]{2,64}) (HTTP/[0-9].[0-9])\n(([a-zA-Z0-9.-]{1,128}):([ -~]{0,128})\n)*\n(?:.*)$";

    //Parsing Request Line
    const char *request_expr = "^([a-zA-Z]{0,8}) /([a-zA-Z0-9.-]{2,64}) (HTTP/[0-9].[0-9])\r\n";
    int bytes = parsing_function(request, string, request_expr, 0);
    if (bytes == -1) {
        printf("\nRequest Line match failed for string : '%s'", string);
        return response(request, 400); //BAD REQUEST
    }
    bytes += 5; //for 2 spaces + '\' in front of uri + '\r\n'

    //Parsing Each Header File
    int num = 0;
    request_expr = "^([a-zA-Z0-9.-]{1,128}): ([ -~]{0,128})\r\n";
    curr = parsing_function(request, string + bytes, request_expr, 1);
    while (curr != -1) {
        bytes += curr + 4; //for 1 colon + space + '\r\n'
        num++;
        curr = parsing_function(request, string + bytes, request_expr, 1);
    }

    // Check that next two chars are '\r\n'
    if ((string[bytes] != '\r' && string[bytes] != '\n')
        | (string[bytes + 1] != '\n' && string[bytes + 1] != '\r')) {
        printf("\nFailed the \\r\\n check, ord(string[bytes]) : %d, ord(string[bytes+1]) : %d",
            string[bytes], string[bytes + 1]);
        return response(request, 400);
    }
    bytes += 2; //for '\r\n'

    //Puts rest of string into msg, if there is more
    if (length > bytes) { //there is more in string
        request->msg = (char *) calloc(length - bytes, sizeof(char));
        string = string + bytes;
        memcpy(request->msg, string, length - bytes);
    }

    // printf("request.method is now: %s\n", request->method);
    // printf("request.uri is now: %s\n", request->uri);
    // printf("request.version is now: %s\n", request->version);
    // printf("request.len_msg is now: %d\n", request->len_msg);
    // printf("request.msg is now: '%s'\n", request->msg);

    // printf("\nCalling get() put() or error()");

    if (strcmp(request->method, "GET") != 0
        && strcmp(request->method, "PUT") != 0) //check if method is not GET nor PUT
        return response(request, 501);

    if (strcmp(request->version, "HTTP/1.1") != 0)
        return response(request, 505);

    if (strcmp(request->method, "GET") == 0) {
        if (request->msg
            != NULL) //msg body with a get command. Since buffer size is 2049, if there is a msg last char of buffer will be occupied at the least (since request line + header files is at most 2048)
            return response(request, 400);
        if (access(request->uri, F_OK) != 0) //file dne
            return response(request, 404);
        struct stat file_stat; //checks if it is a directory
        if (stat(request->uri, &file_stat) == 0 && S_ISDIR(file_stat.st_mode)) {
            printf("%s is a directoru, calling 403 error", request->uri);
            return response(request, 403);
        }
        if (access(request->uri, R_OK) != 0) { //read permissions not enabled
            printf("%s is a directory, calling 403 error", request->uri);
            return response(request, 403);
        }

        else
            return get(request);
    }

    //now we know it must be a PUT
    if (access(request->uri, F_OK) == 0
        && access(request->uri, W_OK & R_OK) != 0) //file exists but does not have write permissions
        return response(request, 403);

    return put(request, length - bytes);

    return 0;
}

//sets up server, calls buffer_processing function, frees request,
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Incorrect Usage");
        return 1;
    }

    for (size_t i = 0; i < strlen(argv[1]); i++) {
        if (isdigit((argv[1])[i]) == 0) { //checking if each character of argv[1] is a digit
            return printf("Invalid Command\n");
            return 1;
        }
    }
    int port = atoi(argv[1]);
    if (port < 1 || port > 65535) {
        printf("Invalid port number");
        return 1;
    }

    Listener_Socket sock;

    int result = listener_init(&sock, port);
    if (result == -1) {
        printf("Could not intialize socket");
        return 1;
    }

    while (1) {
        int fd = listener_accept(&sock);
        if (fd == -1) {
            printf("Could not accept socket");
            return 1;
        }

        Request request;
        create_request(&request);
        request.fd = fd;

        char *buffer = calloc(
            SIZE, sizeof(char)); //maybe use malloc bc binary data dont wanna initialize to 0
        int read_result = read_until(fd, buffer, SIZE, "\r\n\r\n");
        if (read_result == -1) {
            printf("\nread= -1");
            int read_error = errno; //if it is a timeout return # of bytes read
            if (read_error != 11) //errno of 11 means timeout
                response(&request, 500);
        }

        process(&request, buffer, read_result);
        //read_result stores the length of buffer, since we're using binary data and cannot check for it using strlen()
        free(buffer);
        char new_buffer[SIZE2];
        //works but buffer is not rewritten each time (contains last input, but it doens't matter this time since we're not doing anything with the buffer)
        // Read rest of file here -- there would be left over if get() and msg or error occured
        do {
            read_result = my_read_n_bytes(request.fd, new_buffer, SIZE2);
            if (read_result == -1) {
                printf("\nread= -1, calling response(500)");
                //response(&request, 500);
            }
        } while (read_result > 0);
        close(request.fd);

        free_request(&request);

        fflush(stdout);
    }

    return 0;
}
