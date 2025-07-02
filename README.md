![Build](https://github.com/fredrikwidlund/libdynamic/actions/workflows/c-cpp.yml/badge.svg)

## About

libdynamic is a single file header library designed to provide dynamic, performance optimized, low-level data structures for C.

## Installation

Copy `dynamic.h` to your header path and compile with `-flto` to ensure link time optimizations.

## Unit tests

The test suite requires cmocka and valgrind, and requires 100% line and branch coverage to succeed.

```
./configure
make check
```

## Versioning

libdynamic follows the semantic versioning scheme.

## Licensing

libdynamic is licensed under the zlib license.

## Data types

### Data containers

Data containers are generic data containers reducing the need for the common use case of handling separate variables for pointers and size. Data vectors also remove the need for zero-terminated strings, and help reduce strlen() runtime usage.

### Strings

UTF-8 compliant strings that store an explicit string length to avoid zero termination issues.

### Vectors

Vectors are dynamically resized arrays, similar to C++ std::vector, with O(1) random access, and O(1) inserts and removals at the end.

### Lists

Lists are doubly linked sequence containers, similar to C++ std::list, with O(1) inserts (given a known position) and deletes.

### Buffers

Buffers offers generic data containers with dynamic memory allocation. Buffers can inserted into, erased from, resized and compacted, saved to and loaded from files.

### Memory pools

Memory pools improves performance when frequently allocating and deallocating objects of a predetermined size.
