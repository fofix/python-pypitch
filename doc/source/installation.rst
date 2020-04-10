Installation
============

Install package with pip
------------------------

To install with pip::

    $ pip install pypitch

Install from source
-------------------

To install pypitch from the latest source, first obtain the source code::

    $ git clone https://github.com/fofix/python-pypitch
    $ cd python-pypitch

then build the extension::

    $ python setup.py build_ext --inplace

then install with::

    $ pip install .

or::

    $ pip install -e .

for development.


Dependencies
------------

Python Packages
^^^^^^^^^^^^^^^

To use this package, you will need ``Cython``.


Compiler Toolchain
^^^^^^^^^^^^^^^^^^

The same compiler toolchain used to build the CPython interpreter should also
be available. Refer to the
`CPython Developer's Guide <https://devguide.python.org/setup/#build-dependencies>`_
for details about the compiler toolchain for your operating system.
