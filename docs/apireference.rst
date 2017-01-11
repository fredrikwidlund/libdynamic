.. _apireference:

*************
API Reference
*************

.. highlight:: c

Library Version
===============

The libreactor version uses `Semantic Versioning`_ and is of the form *A.B.C*, where *A* is the major
version, *B* is the minor version and *C* is the patch version.

When a new release only fixes bugs and doesn't add new features or
functionality, the patch version is incremented. When new features are
added in a backwards compatible way, the minor version is incremented
and the micro version is set to zero. When there are backwards
incompatible changes, the major version is incremented and others are
set to zero.

.. _`Semantic Versioning`: http://semver.org/

The following preprocessor constants specify the current version of
the library:

``LIBREACTOR_VERSION_MAJOR``, ``LIBREACTOR_VERSION_MINOR``, ``LIBREACTOR_VERSION_PATCH``
  Integers specifying the major, minor and patch versions,
  respectively.

``LIBREACTOR_VERSION``
  A string representation of the current version, e.g. ``"1.2.1"``

Buffer
======

A buffer object represents raw memory that is dynamically increased when data is inserted. The amount
of memory actually allocated will grow exponentially to allow for amortized constant time appends.

.. type:: buffer

  This data structure represents the buffer object.

.. function:: void buffer_construct(buffer *buffer)

  Initializes an empty buffer.

.. function:: void buffer_destruct(buffer *buffer)

  Releases all resources used by the buffer.

.. function:: size_t buffer_size(buffer *buffer)

  Returns the size of the memory contained in the buffer.

.. function:: size_t buffer_capacity(buffer *buffer)

  Returns the amount of memory allocated for the buffer. 

.. function:: void buffer_reserve(buffer *buffer, size_t size)
              
  Ensure that the buffer capacity is at least *size* bytes large.

.. function:: void buffer_compact(buffer *buffer)

  Reduces the amount to allocated memory to match the current buffer size.

.. function:: void buffer_insert(buffer *buffer, size_t position, void *data, size_t size)

  Inserts *data* with a given *size* into the given *position* of the *buffer* 

  Inserts the memory referenced by *data*, of size *size*, into the buffer memory at position *position*.


  void    buffer_insert_fill(buffer *, size_t, size_t, void *, size_t);
void    buffer_erase(buffer *, size_t, size_t);
void    buffer_clear(buffer *);
void   *buffer_data(buffer *);
