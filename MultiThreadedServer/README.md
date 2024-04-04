## httpserver.c: 
creates a multithreaded server for client to send GET and PUT commands to 

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
