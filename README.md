libdynamic
==========

Dynamic C containers for C, with a focus on simplicy and performance.

Where appropriate containers are modelled (roughly) after the stdc++ equivalents.

Currently implemented types are:

* vector  - modelled after std::vector
* string  - modelled after std::string

Installation
------------

    ./autogen.sh
    ./configure
    make
    sudo make install

Tests
-----

Requires cmocka (http://cmocka.org/) to be installed, as well as valgrind (http://valgrind.org/) for memory tests.

    make check

The tests require 100% code coverage to succeed.

    $ cat test/test_coverage.sh.log 
    [buffer]
    File 'src/buffer.c'
    Lines executed:100.00% of 70
    Branches executed:100.00% of 14
    Taken at least once:100.00% of 14
    Calls executed:100.00% of 7
    [vector]
    File 'src/vector.c'
    Lines executed:100.00% of 59
    Branches executed:100.00% of 6
    Taken at least once:100.00% of 6
    Calls executed:100.00% of 25
    [string]
    File 'src/string.c'
    Lines executed:100.00% of 94
    Branches executed:100.00% of 24
    Taken at least once:100.00% of 24
    Calls executed:100.00% of 48
