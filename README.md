# PyPitch

[![Build Status](https://travis-ci.org/fofix/python-pypitch.svg?branch=master)](https://travis-ci.org/fofix/python-pypitch)
[![Build status](https://ci.appveyor.com/api/projects/status/0f6yb99cd37v6li6?svg=true)](https://ci.appveyor.com/project/Linkid/python-pypitch)
[![Documentation Status](https://readthedocs.org/projects/pypitch/badge/?version=latest)](http://pypitch.readthedocs.io/en/latest/?badge=latest)


PyPitch is a C++-extension in Python to analyse audio streams for pitch.


## Setup

Build the extension:

    $ python setup.py build_ext --inplace --force

Install it:

    $ python -m pip install .


## Usage

Import it:

    from pypitch import pypitch


## Doc

To build the html doc from source::


    $ pip install -e .[docs]
    $ cd doc
    $ make html
