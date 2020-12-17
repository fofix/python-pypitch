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
extra_compile_args_pitch = list()
extra_link_args_pitch = list()
if sys.platform.startswith("darwin"):
    # hack: with mac os version 10.13.6 (high sierra), xcode 10 requires the use of the libc++ standard library
    extra_compile_args_pitch.extend(["-stdlib=libc++", "-mmacosx-version-min=10.9"])
    extra_link_args_pitch.extend(["-stdlib=libc++", "-mmacosx-version-min=10.9"])

ext = Extension(
    name='pypitch.pypitch',
    sources=ext_sources,
    language='c++',
    extra_compile_args=extra_compile_args_pitch,
    extra_link_args=extra_link_args_pitch,
)

# setup
setup(
    name='pypitch',
    version='1.3',
    description='PyPitch analyses audio streams for pitch',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='FoFiX team',
    author_email='contact@fofix.org',
    license='GPLv2+',
    url='https://fofix.org',
    project_urls={
        'Documentation': 'https://pypitch.readthedocs.io',
        'Source Code': 'https://github.com/fofix/python-pypitch',
        'Bug Tracker': 'https://github.com/fofix/python-pypitch/issues',
    },
    packages=['pypitch'],
    #package_data={'pypitch': ['*.dll']},
    zip_safe=False,
    classifiers=[
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU General Public License v2 or later (GPLv2+)',
        "Programming Language :: C++",
        "Programming Language :: Cython",
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
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
    tests_require=[
        "pytest<5;python_version<'3.4'",
        "pytest;python_version>'3.4'",
        "numpy<1.17;python_version<'3.4'",
        "numpy<1.20;python_version=='3.6'",
        "numpy;python_version>'3.6'",
    ],
    extras_require={
        'tests': ["pytest<5;python_version<'3.4'", "pytest;python_version>'3.4'", "numpy<1.17;python_version<'3.4'", "numpy<1.20;python_version=='3.6'", "numpy;python_version>'3.6'"],
        'docs': ['sphinx', 'sphinx_rtd_theme'],
    },
)
