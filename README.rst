libdynamic v2.0
===============

.. image:: https://travis-ci.org/fredrikwidlund/libdynamic.svg?branch=master
  :target: https://travis-ci.org/fredrikwidlund/libdynamic
    
.. image:: https://coveralls.io/repos/github/fredrikwidlund/libdynamic/badge.svg?branch=master
  :target: https://coveralls.io/github/fredrikwidlund/libdynamic?branch=master
  
.. image:: https://img.shields.io/lgtm/grade/cpp/g/fredrikwidlund/libdynamic.svg?logo=lgtm&logoWidth=18)
  :target: https://lgtm.com/projects/g/fredrikwidlund/libdynamic/context:cpp

.. image:: https://readthedocs.org/projects/libdynamic/badge/?version=latest
  :target: http://libdynamic.readthedocs.io/en/latest/?badge=latest
  :alt: Documentation Status

Documentation is available at http://libdynamic.readthedocs.io/en/latest/.

Description
===========

Status
======

Build
=====

Build from the Git repository
-------------------------

Building from Git requires GNU Autotools (autoconf_, automake_, libtool_).

.. code-block:: shell

    $ ./autogen.sh
    $ ./configure
    $ make install
    
Build from release
------------------

.. code-block:: shell

    $ ./configure
    $ make install

Running unit tests
------------------

The test suite requires cmocka_ and valgrind_.

.. code-block:: shell

    $ ./configure
    $ make check

.. _cmocka: https://cmocka.org/
.. _valgrind: http://valgrind.org/
.. _autoconf: http://www.gnu.org/software/autoconf/
.. _automake: http://www.gnu.org/software/automake/
.. _libtool: http://www.gnu.org/software/libtool/
.. _benchmarks: https://github.com/fredrikwidlund/libdynamic_benchmark
