#!/bin/sh

#for file in hash buffer list vector string maps mapi map pool
for file in buffer core hash list map mapi maps pool string vector segment utility
do
    echo [$file]
    test=`gcov -b src/dynamic/libdynamic_test_a-$file | grep -A4 File.*$file`
    echo "$test"
    echo "$test" | grep '% of' | grep '100.00%' >/dev/null || exit 1
    echo "$test" | grep '% of' | grep -v '100.00%' >/dev/null && exit 1
done
exit 0
