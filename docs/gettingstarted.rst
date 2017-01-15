***************
Getting Started
***************

.. highlight:: c

Compiling and installing libdynamic
===================================

The libdynamic source is available at
https://github.com/fredrikwidlund/libdynamic/releases/download/v1.0.0/libdynamic-1.0.0.tar.gz

Unpack the source tarball and change to the source directory:

.. parsed-literal::

    $ tar xfz libdynamic-|release|.tar.gz
    $ cd libdynamic-|release|

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

Building the documentation
==========================

(This subsection describes how to build the HTML documentation you are
currently reading, so it can be safely skipped.)

Documentation is in the ``docs/`` subdirectory. It's written in
reStructuredText_ with Sphinx_ annotations. To generate the HTML
documentation, invoke:

.. code-block:: shell

    $ make html

and point your browser to ``doc/_build/html/index.html``. Sphinx_ 1.0
or newer is required to generate the documentation.

.. _reStructuredText: http://docutils.sourceforge.net/rst.html
.. _Sphinx: http://sphinx.pocoo.org/


Compiling programs that use libdynamic
======================================

libdynamic headers files are included through one C header file, :file:`dynamic.h`, so it's enough
to put the line

::

    #include <dynamic.h>

in the beginning of every source file that uses libdynamic.

There's also just one library to link with, ``libdynamic``. libdynamic is built as a static library
and should be compiled with LTO_ (link time optimization) to provide the best performance. Compile and
link the program as follows:

.. code-block:: shell

    $ cc -o prog prog.c -flto -fuse-linker-plugin -ldynamic

Use of pkg-config_ is supported and recommended:

.. code-block:: shell

    $ cc -o prog prog.c `pkg-config --cflags --libs libdynamic`

.. _LTO: https://en.wikipedia.org/wiki/Interprocedural_optimization
.. _pkg-config: http://pkg-config.freedesktop.org/
