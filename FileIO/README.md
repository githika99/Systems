## memory.c: 
provides a get/set memory abstraction for files in a Linux directory

## Makefile: 
creates the executable memory

## Use:
### make all - produces the memory binary
### make memory - produces the memory binary
### make clean - removes all .o and binary files
### make format - clang-formats memory.c

### Example Input:
##### ./memory < input.txt

### Contents of input.txt for a set command
set<br>
[output file]<br>
[# of bytes you want]<br>
[content you want to but into output file]<br>
### This will set the frist # of bytes of contents to output file 

### Contents of input.txt for a get command 
get<br>
[output file]<br>
### This will output the contents of get to stdout

## Testing
./test_repo.sh to run all tests<br>
./test_scripts/test_name.sh to run that particular test <br>


### Author: Githika Annapureddy
### Date: 1/20/2024
