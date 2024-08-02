libdynamic v0.9
===============

.. image:: https://coveralls.io/repos/github/fredrikwidlund/libdynamic/badge.svg?branch=master
  :target: https://coveralls.io/github/fredrikwidlund/libdynamic?branch=master

Description
===========

libdynamic is a group of single file libraries for C, implementing high performance data types such as buffers, lists, vectors and maps. It is used for example in libreactor_.

Build
=====

Include header files where needed and link with -flto to avoid code duplication.

Running unit tests
------------------

The test suite requires cmocka_ and valgrind_.

.. code-block:: shell

    ./configure
    make check
    
Versioning
==========

libdynamic follows the `semantic versioning`_ scheme.

Licensing
=========

libdynamic is licensed under the zlib license. 

.. _libreactor: https://github.com/fredrikwidlund/libreactor
.. _`semantic versioning`: https://semver.org/
.. _cmocka: https://cmocka.org/
.. _valgrind: http://valgrind.org/
.. _autoconf: http://www.gnu.org/software/autoconf/
.. _automake: http://www.gnu.org/software/automake/
.. _libtool: http://www.gnu.org/software/libtool/
