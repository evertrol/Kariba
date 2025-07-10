[TOC]

# Testing Kariba

## Building Kariba, the examples and tests

Building the library, its examples and its tests is the first step to
testing Kariba.

Pay attention to warnings. Most, essentially all, warnings should be
fixed. For example, unused variables can indicate typos.

Once everything looks like it's compiling properly, do check one last
time from a clean build, and build in parallel (so you notice if you
have file dependencies incorrectly set):

```
make distclean
make -j
```

Also verify that the order of include files does not matter: you
should be able to swap some of the includes (in particular, the Kariba
ones) without causing a problem. In the end, though, the formatter
will sort the includes alphabetically.


## Unit tests

Kariba has a set of unit tests. These are far from complete, and are
derived from the examples.

Feel free to add further tests. Keep them simple: test basic
functionality first.

If you write a new function, it is good to add unit tests for that
function. Test for a few regular input arguments, but also for bad
arguments (negative values, empty vectors etc).

If you find a bug, and fix the bug, add a unit test that specifically
tests for this bug, so that the bug will be caught quicker and more
easily should it return (a so-called regression test).

## Examples

The examples are tested by running them, then comparing the *.dat
files in the output/ directory with those in the comparison-output/
directory. The differences should be exactly zero. A quick one-liner
in bash or zsh is `for path in output/*dat ; do fname=$(basename
$path) ; comppath="comparison-output/${fname}" ; echo "diff -q $path
$comppath" ; diff -q $path $comppath ; done`.


## GitHub actions

All of the above (or nearly; not the shuffling of include files) is
done automatically as part of the continuous integration (CI). This is
done through GitHub actions, and runs in a virtual Linux machine. See
the setup in `.github/workflows/ci.yml`.
