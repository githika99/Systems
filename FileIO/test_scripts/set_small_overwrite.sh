#!/usr/bin/env bash

# This script performs a simple set request of an existing file.


source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

if [[ check_dir -eq 1 ]]; then
    exit 1
fi

## To simplify cleanup, add all files that your script uses.
### You can do this ahead of time, or as you create them.
new_files=""

## setup a file to be overwritten
echo "I have a lovely bunch of coconuts" > test.txt

## Create command file
test_file=test_files/small_ascii.txt
size=$(wc -c $test_file | awk '{print $1}')
printf "set\ntest.txt\n$size\n" > command

new_files="$new_files test.txt command"

## run get command and save to file:
cat command $test_file | ./memory > output.txt 2>err.log
mem_rc=$?
new_files="$new_files output.txt err.log"



## Check each of the outputs:
## (1) memory's return code is 0
## (2) memory's stdout is empyt
## (3) memory wrote the correct content to test.txt
## If any of these fail, produce a helpful message and set rc=1

### Note: we don't check for stderr being empty; you can use it for
### logging should you like!

rc=0
msg=""

## check if memory's return code is 0
if [[ $mem_rc -ne 0 ]]; then
    msg=$'Memory returned non-zero error code\n'
    rc=1
fi


diff output.txt <(echo "OK") > output_diff.txt
output_diff_rc=$?
if [[  $output_diff_rc -ne 0 ]]; then
    msg=$"${msg}Memory produced the wrong output to stdout"$'\n'
    rc=1
fi
new_files="$new_files output_diff.txt"


diff test.txt test_files/small_ascii.txt > diff.txt
diff_rc=$?
if [[ $diff_rc -ne 0 ]]; then
    msg=$"${msg}Memory wrote test.txt incorrectly"$'\n'
    rc=1
fi
new_files="$new_files diff.txt"

if [[ $rc -eq 0 ]]; then
    echo "It worked!"
else
    echo "--------------------------------------------------------------------------------"
    echo "$msg"
    echo "--------------------------------------------------------------------------------"
    echo "return code:"
    echo $mem_rc
    echo "--------------------------------------------------------------------------------"
    echo "stdout:"
    cat output.txt
    echo "--------------------------------------------------------------------------------"
    echo "stderr:"
    cat err.log
    echo "--------------------------------------------------------------------------------"
    if [[ $diff_rc -eq 0 ]]; then
	echo "test.txt is correct."
    else
	echo "test.txt's difference from correct:"
	cat diff.txt
    fi
    echo "--------------------------------------------------------------------------------"
    if [[ $output_diff_rc -eq 0 ]]; then
	echo "stdout is correct."
    else
	echo "stdout's difference from correct:"
	cat output_diff.txt
    fi
    echo "--------------------------------------------------------------------------------"


fi

cleanup $new_files
exit $rc
