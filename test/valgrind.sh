#!/bin/sh

if command -v valgrind; then
    for file in tests
    do
        echo [$file]
        if ! valgrind --track-fds=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all test/$file; then
            exit 1
        fi
    done
fi
