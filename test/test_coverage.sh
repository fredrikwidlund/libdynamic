#!/bin/sh

for file in buffer vector
do
    echo [$file]
    gcov -b src/libdynamic_test_a-$file | grep -A4 File.*$file
done
exit 0

