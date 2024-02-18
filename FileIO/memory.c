/*
Githika Annapureddy
CruzId: gannapur
Assignment: pa1
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
#define SIZE 4096 //change to 4096

int error_msg(const char *input) {
    write(2, input, strlen(input));
    return 1;
}

bool is_filename(char *filename) {
    size_t length = strlen(filename);
    if (length > PATH_MAX)
        return false;
    //for(size_t i = 0; i < length; i++){                 //isn't doing anything bc strtok parses until it reaches a null character. so a null character would not be in the filename.
    //    if (ord(filename[i]) == 0)                   //if its a null character
    //        return false;
    //}
    return true;
}

//checks if the file exists in the current working directory
bool file_exist(char *filename, int w) {
    //no write
    if (w == 0 && access(filename, R_OK) == 0) //file exists and has read permissions enabled
        return true;
    //yes write
    else if (w == 1 && access(filename, W_OK) == 0)
        return true;
    return false;
}

//write first total_bytes of what is in buffer to filename
//buffer may be partway used, carry p (where buffer is, into this)
//TODO: do not read /0 character from first remaining buffer read
//i think call handles this bc strlen(buffer + byte) counts chars until but not including the '/0'
int set(char filename[], char buffer[], int remaining_bytes_first_buffer, int total_bytes) {
    //may be a problem with buffer in parameter and buffer in read() funct
    char *okay = "OK\n";
    int remaining_bytes = 0;
    int bytes_to_write = remaining_bytes_first_buffer;
    if (!is_filename(filename))
        return error_msg("Invalid Command\n"); //not valid filename
    if (access(filename, F_OK) == 0 && access(filename, W_OK) != 0)
        return error_msg("Invalid Command\n"); //file exists but does not have write permissions
    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0666); //added O_TRUNC
    if (fd == -1)
        return error_msg("Operation Failed\n"); //used to be "File could not be opened/created"
    char *p = buffer;
    char new_buffer[PATH_MAX];

    while (total_bytes > 0) {
        //on first run, total_bytes will be < SIZE
        //write whats smaller total bytes or whole buffer
        while (bytes_to_write > 0) {
            //here, make sure it is rest of buffer (keep track of where we left off)
            int written = write(fd, p, bytes_to_write);
            if (written == -1)
                return error_msg("Operation Failed\n"); //used to be "error writing to file"

            bytes_to_write -= written;
            total_bytes -= written;
            p += written;
        }
        if (total_bytes > 0) {
            remaining_bytes = read(0, new_buffer, PATH_MAX); //not adding a null /0 here
            if (remaining_bytes == -1)
                return error_msg("Invalid Command\n"); //used to be "Unsuccesful Read"
            if (remaining_bytes == 0) {
                break;
            }
            p = new_buffer;
        }

        //first condition should only be active on last buffer read, that's the only time remaining_bytes should be less than PATH_MAX
        if (remaining_bytes < PATH_MAX && remaining_bytes < total_bytes)
            bytes_to_write = remaining_bytes;
        else if (total_bytes < PATH_MAX)
            bytes_to_write = total_bytes;
        else
            bytes_to_write = PATH_MAX; //will be whole buffer if buffer is full
    }

    close(fd);

    int w = write(1, okay, strlen(okay)); //write 'OK' to filename
    if (w == -1)
        return error_msg("Operation Failed\n"); //unnecessary error message?
    return 0;
}

//write contents of filename to stdout using write()
//steps: open file, read file contents, write contents to stdout
//keep track of how many bytes read has read at each step
int get(char *filename) {
    if (!is_filename(filename) || !file_exist(filename, 0))
        return error_msg(
            "Invalid Command\n"); //used to be File does not exist or does not have read permissions.

    int fd = open(filename, O_RDONLY, 0000);
    if (fd == -1)
        return error_msg("Invalid Command\n"); //Used to be File cannot be opened

    char buffer[SIZE];
    while (1) {
        int remaining_bytes = read(fd, buffer, SIZE);

        if (remaining_bytes == -1)
            return error_msg("Invalid Command\n"); //used to be "Unsuccessful Read"

        if (remaining_bytes == 0)
            break;

        char *p = buffer;

        while (remaining_bytes > 0) {
            int bytes_written = write(1, p, remaining_bytes); //remaining_bytes is capped by SIZE

            if (bytes_written == -1)
                return error_msg("Operation Failed\n"); //used to be "Unsuccessful write\n"

            remaining_bytes -= bytes_written; //if equal, loop stops
            p += bytes_written;
        }
    }
    close(fd);
    return 0;
}

int main(void) {
    /*
    char filename[PATH_MAX + 1] = "examle3.txt";
    char buffer[PATH_MAX + 20] = "this is the text I want to write\n";
    printf("calling set\n");
    set(filename, buffer, strlen(buffer));
    */

    int count = 0;
    int total_bytes = 0;
    int s = 0;
    int g = 0;
    char filename[PATH_MAX + 1];
    char buffer[PATH_MAX + 20];
    char lst[1];
    char *okay = "OK\n";
    int bytes_read = 0;
    int byte = 0;
    char *p = buffer;
    int remaining_bytes = 0;

    while (bytes_read < PATH_MAX + 20) {
        remaining_bytes = read(0, p, PATH_MAX + 20 - 1 - bytes_read);
        if (remaining_bytes == -1)
            return error_msg("Invalid Command\n"); //use to be "Unsuccesful Read\n"

        if (remaining_bytes == 0) {
            break; //read in nothing
        }
        bytes_read += remaining_bytes;
        p += remaining_bytes;
    }
    if (bytes_read == 0)
        return error_msg("Invalid Command\n");

    lst[0] = buffer[bytes_read - 1]; //record last character in buffer
    buffer[bytes_read] = '\0';

    char *word = strtok(buffer, "\n");
    while (word != NULL) {
        if (count == 0) {
            if (strcmp(word, "get") == 0)
                g = 1;
            else if (strcmp(word, "set") == 0)
                s = 1;
            else
                return error_msg("Invalid Command\n");

            count++;
            byte += strlen(word) + 1; //for 'get\n'
        } else if (g == 1) {
            if (count == 1) {
                strcpy(filename, word);
                byte += strlen(word) + 1;
                count++;
            } else
                return error_msg("Invalid Command\n");
        } else if (s == 1) {
            if (count == 1) {
                strcpy(filename, word);
                byte += strlen(word) + 1;
                count++;
            } else if (count == 2) {
                for (size_t i = 0; i < strlen(word); i++) {
                    if (isdigit(word[i]) == 0)
                        return error_msg("Invalid Command\n");
                }
                total_bytes = atoi(word);
                byte += strlen(word) + 1;
                count++;
                break;
            }
        }

        word = strtok(NULL, "\n");
    }

    if (g && count == 2) {
        if (lst[0] != '\n')
            return error_msg("Invalid Command\n"); //last character is not newline
        return get(filename);
    } else if (s && count == 3) {
        int remaining_bytes_in_first_buffer = bytes_read - byte;
        if (remaining_bytes_in_first_buffer == 0
            && (lst[0]
                != '\n')) //nothing after number, but check if newline character was after number
            return error_msg("Invalid Command\n"); //used to be "last character not newline"

        else if (remaining_bytes_in_first_buffer == 0) { //nothing after number
            //create a newfile but don't make any changes
            int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0666); //added O_TRUNC
            if (fd == -1)
                return error_msg(
                    "Operation Failed\n"); //used to be "File could not be opened/created"

            int w = write(1, okay, strlen(okay)); //write 'OK' to filename
            if (w == -1)
                return error_msg("Operation Failed\n"); //unnecessary error message?

        } else {
            return set(filename, buffer + byte, remaining_bytes_in_first_buffer, total_bytes);
        }

    } else
        return error_msg("Invalid Command\n");

    (void) total_bytes;

    return 0;
}
