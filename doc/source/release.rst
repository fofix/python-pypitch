How to release
==============

Create a release
----------------

Here is the release process for ``PyPitch``:

#. bump the version (will commit)::

    python -m pip install bumpversion
    bumpversion release

#. update the changelog date (in the same commit)::

    vim CHANGELOG.md
    git add CHANGELOG.md
    git commit --amend

#. make a PR

#. if tests pass on Travis, merge the PR

#. make a tag::

    git tag VERSION

#. make the source distribution::

    python setup.py build_ext --inplace --force
    python setup.py sdist

#. make wheels with AppVeyor

#. upload sources and wheels on GitHub in a new release.


Upload to PyPI
--------------

You need to upload sources and wheels to PyPI::

    python -m pip install twine
    twine upload -r pypi dist/*

.. note::

    First, upload on `TestPyPI`_::

        $ twine upload -r pypitest dist/*


Then, you need to test the installation::

    pushd $(mktemp -d)
    mktmpenv
    pip install -i https://test.pypi.org/simple pypitch
    python -c "from pypitch import pypitch; print(pypitch.__version__)"
    deactivate
    popd


Prepare the next release
------------------------

To prepare the next release:

#. bump the version (with PART in [major, minor, patch], will commit)::

    bumpversion PART

#. update the changelog to add the unreleased subsection (in the same commit)::

    vim CHANGELOG.md
    git add CHANGELOG.md
    git commit --amend

#. make a PR.


.. _TestPyPI: https://test.pypi.org
