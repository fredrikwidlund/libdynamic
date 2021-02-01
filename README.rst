libdynamic v2.3
===============

.. image:: https://travis-ci.org/fredrikwidlund/libdynamic.svg?branch=master
  :target: https://travis-ci.org/fredrikwidlund/libdynamic
    
.. image:: https://coveralls.io/repos/github/fredrikwidlund/libdynamic/badge.svg?branch=master
  :target: https://coveralls.io/github/fredrikwidlund/libdynamic?branch=master
  
.. image:: https://img.shields.io/lgtm/grade/cpp/g/fredrikwidlund/libdynamic.svg?logo=lgtm&logoWidth=18)
  :target: https://lgtm.com/projects/g/fredrikwidlund/libdynamic/context:cpp

Documentation is available at http://libdynamic.readthedocs.io/en/latest/.

Description
===========

libdynamic is a utility library for C that will give you dynamic data structures like buffers, lists, vectors, maps and strings. It also includes asynchronous worker pools, and a core event driven framework. It is used, for example, in conjunction with libreactor_ handling many millions of HTTP transactions daily for over 5 years.

Status
======

libdynamic currently is (and has been for many years) used in high concurrency and high performance production environments in a very robust manner.

Build
=====

Build from the Git repository
-----------------------------

Building from Git requires GNU Autotools (autoconf_, automake_, libtool_).

.. code-block:: shell

    git clone https://github.com/fredrikwidlund/libdynamic
    cd libdynamic
    ./autogen.sh
    ./configure
    make install
    
Build from release
------------------

.. code-block:: shell

    wget https://github.com/fredrikwidlund/libdynamic/releases/download/v2.3.0/libdynamic-2.3.0.tar.gz
    tar fxz libdynamic-2.3.0.tar.gz
    cd libdynamic-2.3.0
    ./configure
    make install

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

libdynamic is licensed under the MIT license. 

.. _libreactor: https://github.com/fredrikwidlund/libreactor
.. _`semantic versioning`: https://semver.org/
.. _cmocka: https://cmocka.org/
.. _valgrind: http://valgrind.org/
.. _autoconf: http://www.gnu.org/software/autoconf/
.. _automake: http://www.gnu.org/software/automake/
.. _libtool: http://www.gnu.org/software/libtool/
.. _benchmarks: https://github.com/fredrikwidlund/libdynamic_benchmark
