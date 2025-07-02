#!/bin/sh

for file in tests
do
    echo [$file]
    test=`gcov -b "test/$file" | grep -A4 "File.*$file\|src/dynamic.h"`
    echo "$test"
    echo "$test" | grep '% of' | grep '100.00%' >/dev/null || exit 1
    echo "$test" | grep '% of' | grep -v '100.00%' >/dev/null && exit 1
done
exit 0
