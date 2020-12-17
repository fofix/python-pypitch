Release notes
=============

1.3 (2020-12-17)
----------------

- Python 3.9 compatibility
- Setup: update contact information
- CI: replace Travis with Github Actions
- Code: remove the `sqrLT` function


1.2 (2020-04-12)
----------------

- Python 3 compatible (CI, code, tests)
- Code: use `std::fmod` instead of `std::remainder` only for Visual C++ 9.0 (Windows & python 2.7)
- Doc: add a release section
- Setup: use ``bumpversion`` to bump the version easily


1.1 (2020-04-10)
----------------

- Doc: initialise some basics
- Setup: correctly use the relative path of the README file
- Setup: declare 'docs' extra for the documentation
- Setup: declare 'tests' extra for tests
- Setup / osx: use libc++ to compile the extension (xcode >= 10)
- Tests: restrict the pytest version (< 5)


1.0 (2020-03-15)
----------------

Initial release. Extracted from FoFiX.
