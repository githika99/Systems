#!/usr/bin/env bash

source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

priority=N_WAY

ANY_RD=1
ONE_WR=2
STRESS=4
TPUT=8

if [[ `check_dir` -eq 1 ]]; then
    exit 1
fi

cfile=rwlock_test.c

cp rwlock_users/$cfile .
clang -Wall -Werror -Wextra -Wpedantic -Wstrict-prototypes -DDEBUG -o rwlock_test rwlock.o $cfile -lpthread

n=5
num_readers=10
num_writers=10
test_time=10

valgrind --error-exitcode=1 --log-file=val1_log.txt ./rwlock_test $priority $n $num_readers $num_writers $test_time >output1.txt 2>error1.txt
output1=$?

n=10
num_readers=20
num_writers=20
test_time=10

valgrind --error-exitcode=1 --log-file=val2_log.txt ./rwlock_test $priority $n $num_readers $num_writers $test_time >output2.txt 2>error2.txt
output2=$?

n=20
num_readers=50
num_writers=50
test_time=20

valgrind --error-exitcode=1 --log-file=val3_log.txt ./rwlock_test $priority $n $num_readers $num_writers $test_time >output3.txt 2>error3.txt
output3=$?

final_return_code=$(( (output1 & STRESS) || (output2 & STRESS) || (output3 & STRESS) ))

new_files="$new_files rwlock_test output1.txt error1.txt output2.txt error2.txt output3.txt error3.txt val1_log.txt val2_log.txt val3_log.txt"

if [[ `wc -l < val1_log.txt` -gt 7 ]]; then
    msg=$"${msg}Server died before we killed it (valgrind detected an error)"$'\n'
    rc=1
fi

if [[ `wc -l < val2_log.txt` -gt 7 ]]; then
    msg=$"${msg}Server died before we killed it (valgrind detected an error)"$'\n'
    rc=1
fi

if [[ `wc -l < val3_log.txt` -gt 7 ]]; then
    msg=$"${msg}Server died before we killed it (valgrind detected an error)"$'\n'
    rc=1
fi

if [ $(($final_return_code)) -eq 0 ]; then
    echo "It worked!"
    final_return_code=0
else
    echo "--------------------------------------------------------------------------------"
    echo "return code: $final_return_code"
    echo "--------------------------------------------------------------------------------"
    if [ ! $(($output1 & $STRESS)) -eq 0 ]; then
        echo "output:"
        cat output1.txt
        echo "--------------------------------------------------------------------------------"
        echo "error:"
        cat error1.txt
        echo "--------------------------------------------------------------------------------"
    fi
    if [ ! $(($output2 & $STRESS)) -eq 0 ]; then
        echo "output:"
        cat output2.txt
        echo "--------------------------------------------------------------------------------"
        echo "error:"
        cat error2.txt
        echo "--------------------------------------------------------------------------------"
    fi
    if [ ! $(($output3 & $STRESS)) -eq 0 ]; then
        echo "output:"
        cat output3.txt
        echo "--------------------------------------------------------------------------------"
        echo "error:"
        cat error3.txt
        echo "--------------------------------------------------------------------------------"
    fi
fi

echo "--------------------------------------------------------------------------------"
    echo "valgrind1's output:"
    cat val1_log.txt
    echo "--------------------------------------------------------------------------------"
    echo "--------------------------------------------------------------------------------"
    echo "valgrind2's output:"
    cat val2_log.txt
    echo "--------------------------------------------------------------------------------"
    echo "--------------------------------------------------------------------------------"
    echo "valgrind3's output:"
    cat val3_log.txt
    echo "--------------------------------------------------------------------------------"


cleanup $new_files
exit $final_return_code
