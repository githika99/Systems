## Purpose: 
creates a multithreaded server for client to send GET and PUT commands to 

## Files:
test_files, test_scripts, and workloads are all tests. <br>
More information is provided in those folders. <br>

List.c is an ADT used to  handle file access across multiple threads <br>

Queue.c is an ADT to order requests <br>

rwlock.c is a Reader Writer Lock ADT that uses mutexes. I am using it to lock file access <br>

The server itself is contained in the files: httpserver.c, my_helpers.c, and my_server.c <br>

asgn4_helper_funcs.a is a series of helper functions provided (they can be viewed in asgn2_helper_funcs.h). I use the socket binding functions: listener_init() and listener_accept() from these files. <br>

## Makefile: 
creates the httpserver memory

## Use:
make all - produces the httpserver binary<br>
make httpserver - produces the httpserver binary<br>
make clean - removes all .o and binary files<br>
make format - clang-formats httpserver.c<br>

## Notes:
The List ADT I am using was written by me in Fall 2023 for CSE 101. I did make changes to it to support two data fields. I also deleted and added funcitons accordingly. <br>
I am also using the Queue and RWLock that I developed for other assignments in CSE 130. 

### Author: Githika Annapureddy
### Date: 3/05/2024
