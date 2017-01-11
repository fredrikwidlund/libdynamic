.. _apireference:

*************
API Reference
*************

.. highlight:: c

Library Version
===============

The libreactor version uses `Semantic Versioning`_ and is of the form *A.B.C*, where *A* is the major version, *B* is
the minor version and *C* is the patch version.

When a new release only fixes bugs and doesn't add new features or functionality, the patch version is incremented.
When new features are added in a backwards compatible way, the minor version is incremented and the micro version is
set to zero. When there are backwards incompatible changes, the major version is incremented and others are set to
zero.

The following preprocessor constants specify the current version of the library:

``LIBREACTOR_VERSION_MAJOR``, ``LIBREACTOR_VERSION_MINOR``, ``LIBREACTOR_VERSION_PATCH``
  Integers specifying the major, minor and patch versions, respectively.

``LIBREACTOR_VERSION``
  A string representation of the current version, e.g. ``"1.2.1"``

Buffer
======

A buffer object represents raw memory that is dynamically increased when data is inserted. The amount of memory
actually allocated will grow exponentially to allow for amortized constant time appends.

.. type:: buffer

  This data structure represents the buffer object.

.. function:: void buffer_construct(buffer *buffer)

  Constructs an empty *buffer*.

.. function:: void buffer_destruct(buffer *buffer)

  Releases all resources used by the *buffer*.

.. function:: size_t buffer_size(buffer *buffer)

  Returns the size of the memory contained in the *buffer*.

.. function:: size_t buffer_capacity(buffer *buffer)

  Returns the amount of memory allocated for the *buffer*. 

.. function:: void buffer_reserve(buffer *buffer, size_t size)
              
  Ensure that the *buffer* capacity is at least *size* bytes large.

.. function:: void buffer_compact(buffer *buffer)

  Reduces the amount of allocated memory in the *buffer* to match the current buffer size.

.. function:: void buffer_insert(buffer *buffer, size_t position, void *data, size_t size)

  Inserts *data* with a given *size* into the given *position* of the *buffer* 

.. function:: void buffer_insert_fill(buffer *buffer, size_t postion, size_t count, void *data, size_t size)

  Inserts *count* copies of *data* with a given *size* into the given *position* of the *buffer*
  
.. function:: void buffer_erase(buffer *buffer, size_t position, size_t size)

  Removes *size* bytes from the data in the *buffer* at the given *position*.

.. function:: void buffer_clear(buffer *buffer)

  Clears the *buffer* of all content.

.. function:: void *buffer_data(buffer *buffer)

  Returns a pointer the the content of the *buffer*.

Vector
======

Vectors are sequence containers representing arrays that can change in size. Vectors are modelled roughtly after the
`C++ vector`_ counterpart.

Just like arrays, vectors use contiguous storage locations for their elements, which means that their elements can also
be accessed using offsets on regular pointers to its elements, and just as efficiently as in arrays. But unlike arrays,
their size can change dynamically, with their storage being handled automatically by the container.

Internally, vectors use a dynamically allocated array to store their elements. This array may need to be reallocated in
order to grow in size when new elements are inserted, which implies allocating a new array and moving all elements to
it. This is a relatively expensive task in terms of processing time, and thus, vectors do not reallocate each time an
element is added to the container.

Instead, vector containers may allocate some extra storage to accommodate for possible growth, and thus the container
may have an actual capacity greater than the storage strictly needed to contain its elements (i.e., its size).
Reallocations only happen at logarithmically growing intervals of size so that the insertion of individual elements at
the end of the vector can be provided with amortized constant time complexity.

Therefore, compared to arrays, vectors consume more memory in exchange for the ability to manage storage and grow
dynamically in an efficient way.

.. _`Semantic Versioning`: http://semver.org/
.. _`C++ vector`: http://www.cplusplus.com/reference/vector/vector/

.. type:: vector

  This data structure represents the vector object.

.. function:: void vector_construct(vector *vector, size_t size)

  Constructs an empty *vector* for elements of the given *size*.

.. function:: void vector_object_release(vector *vector, void (*release)(void *))

  Defines a *release* callback function that is called whenever an element is removed from the *vector*.

.. function:: void vector_destruct(vector *vector)

  Releases all resources used by the *vector*.

.. function:: size_t vector_size(vector *vector)

  Returns the size of the memory contained in the *vector*.

.. function:: size_t vector_capacity(vector *vector)

  Returns the amount of memory allocated for the *vector*. 

.. function:: int vector_empty(vector *vector)

  Returns 1 if the *vector* contains no elements.
  
.. function:: void vector_reserve(vector *vector, size_t size)
              
  Ensure that the *vector* capacity is at least *size* elements.

.. function:: void vector_shrink_to_fit(vector *vector)

  Reduces the amount of allocated memory in the *vector* to match the current vector size.

.. function:: void *vector_at(vector *vector, size_t position)

  Returns a pointer to the element in given *position* in the *vector*.

.. function:: void *vector_front(vector *vector)

  Returns a pointer to the first element in the *vector*.

.. function:: void *vector_back(vector *vector)

  Returns a pointer to the last element in the *vector*.

.. function:: void *vector_data(vector *vector)

  Returns a direct pointer to the memory array used internally by the *vector* to store its owned elements.

  Because elements in the vector are guaranteed to be stored in contiguous storage locations in the same order as
  represented by the vector, the pointer retrieved can be offset to access any element in the array.

.. function:: void vector_push_back(vector *vector, void *object)

  Appends the *object* to the end of the *vector*.
