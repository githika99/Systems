#!/usr/bin/env bash

source_dir=`dirname ${BASH_SOURCE}`
source "$source_dir/utils.sh"

if [[ `check_dir` -eq 1 ]]; then
    exit 1
fi

cfile=better_lock_unlock.c

cp rwlock_users/$cfile .
clang -o rwlock_test rwlock.o $cfile

./rwlock_test >output.txt 2>error.txt
rc=$?

new_files="$cfile rwlock_test output.txt error.txt"

if [[ $rc -eq 0 ]]; then
    echo "It worked!"
else
    echo "Program did not work"
fi
echo "--------------------------------------------------------------------------------"
echo "return code: $rc"
echo "--------------------------------------------------------------------------------"
echo "output:"
cat output.txt
echo ""
echo "--------------------------------------------------------------------------------"
echo "error:"
cat error.txt
echo ""
echo "--------------------------------------------------------------------------------"


cleanup $new_files
exit $rc
