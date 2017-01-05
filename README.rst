README
======

.. image:: https://travis-ci.org/fredrikwidlund/libdynamic.svg?branch=master
  :target: https://travis-ci.org/fredrikwidlund/libdynamic
    
.. image:: https://coveralls.io/repos/github/fredrikwidlund/libdynamic/badge.svg?branch=master
  :target: https://coveralls.io/github/fredrikwidlund/libdynamic?branch=master

Documentation is available at http://libdynamic.readthedocs.io/en/latest/.

Compiling and installing
========================

The source uses GNU Autotools (autoconf_, automake_, libtool_), so
compiling and installing is extremely simple:

.. code-block:: shell

    $ ./configure
    $ make
    $ make install

To run the test suite which requires cmocka_ and valgrind_, invoke:

.. code-block:: shell

    $ make check

To change the destination directory (``/usr/local`` by default), use
the ``--prefix=DIR`` argument to ``./configure``. See ``./configure
--help`` for the list of all possible configuration options.

The command ``make check`` runs the test suite distributed with
libdynamic. This step is not strictly necessary, but it may find possible
problems that libdynamic has on your platform. If any problems are found,
please report them.

If you obtained the source from a Git repository (or any other source
control system), there's no ``./configure`` script as it's not kept in
version control. To create the script, the build system needs to be
bootstrapped. There are many ways to do this, but the easiest one is
to use the supplied autogen.sh script:

.. code-block:: shell

    $ ./autogen.sh

.. _cmocka: https://cmocka.org/
.. _valgrind: http://valgrind.org/
.. _autoconf: http://www.gnu.org/software/autoconf/
.. _automake: http://www.gnu.org/software/automake/
.. _libtool: http://www.gnu.org/software/libtool/
