#!/usr/bin/env python


# from distutils.core import setup
# from distutils.extension import Extension
# from distutils.sysconfig import get_python_lib
from setuptools import setup
from setuptools import Extension
import os
import sys

from Cython.Build import cythonize
# from Cython.Distutils import build_ext


readme_filepath = os.path.join(os.path.dirname(__file__), "README.md")
try:
    import pypandoc
    long_description = pypandoc.convert(readme_filepath, 'rst')
except ImportError:
    long_description = open(readme_filepath).read()


# sources
ext_sources = [
    'pypitch/_pypitch.pyx',
    'pypitch/pitch.cpp'
]

# extension
ext = Extension(
    name='pypitch.pypitch',
    sources=ext_sources,
    language='c++',
)

# setup
setup(
    name='pypitch',
    version='1.0',
    description='PyPitch analyses audio streams for pitch',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='FoFiX team',
    author_email='fofix@perdu.fr',
    license='GPLv2+',
    url='https://github.com/fofix/python-pypitch',
    packages=['pypitch'],
    package_data={'pypitch': ['*.dll']},
    zip_safe=False,
    classifiers=[
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU General Public License v2 or later (GPLv2+)',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Topic :: Multimedia',
        'Topic :: Multimedia :: Sound/Audio',
        'Topic :: Software Development :: Libraries',
    ],
    keywords='pitch audio',
    ext_modules=cythonize(ext, compiler_directives={'language_level': sys.version_info[0]}),
    setup_requires=['cython', 'pytest-runner'],
    install_requires=[
        'Cython >= 0.27',
    ],
    test_suite="tests",
    tests_require=['pytest', 'numpy<1.17'],
)
