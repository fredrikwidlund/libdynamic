#!/bin/sh

if command -v valgrind; then
    for file in buffer vector string map_int map_str
    do
        echo [$file]
        if ! valgrind  --error-exitcode=1 --read-var-info=yes --leak-check=full --show-leak-kinds=all test/test_$file; then
            exit 1
        fi
    done
fi
