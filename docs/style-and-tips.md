[TOC]

# Some style guidelines and tips

## Use the C++ standard library utilities

In particular, containers such as std::vector, std::array,
std::string, std::tuple, etc can be very useful, safe, and often
faster.


Further, use C++ features: e.g., if you are using a struct to group
variables, the struct can be declared and initialized C++ style, not
so much C style:

```
struct Params {
  int a = 1;
  double b = 0.0;
};
Params parameters;
std::cout << parameters.a << "\t" << parameters.b << "\n";
Params parameters2{5};
std::cout << parameters2.a << "\t" << parameters2.b << "\n";
```

(the first line prints `1     -1`, the second `5    -1`.)


## Keep your functions small

If your functions start becoming 100 lines or more, or even exceed 50
lines, consider splitting it up in various subfunctions.

This has multiple advantages

- each function is more readable. This makes finding issues (bugs, or
  parts that can be improved) generally easier

- it makes it easier to write unit tests for the functions

- if the functions are members of a class, it is easier to create an
  overridden member function, so that only a particular algorithm may
  be altered, instead of having to copy-paste a 300-line function into
  a child class and only alter 5 lines.

## Docstrings

Do write documentation strings. It always helps both your future slef
and others to figure out what is going on in the respective function,
or what variables are intended for (but clear variable names also
help). Don't write what the code actually does (the infamous `i++; //
increase i by 1` example), but write what the intent is, the
underlying algorithm and possibly (a) reference(s).

The style for docstrings is Doxygen's Qt style, with docstrings
starting with `/*!` or `//!`. See
https://www.doxygen.nl/manual/docblocks.html for some details, but
generally, follow the overall style in the code.


# Style


This is a general style guide for writing code in the Kariba
library. It can, of course, also be used outside of the library, but
this is not necessary.


### Use the formatter


The code is formatted with `clang-format`; this tool can often be
installed using a package manager. There is a `.clang-format` file
with some definitions for the style (there are many more
options). This ensures a consistent look and feel across the code.

In particular, it will:

- indent consistently (four spaces, except for private/public/protected identifiers)

- break and wrap long lines

- remove trailing whitespace

- ensure a newline at the end of a file

- add spaces around operands (e.g., `a=1+1` becomes `a = 1 + 1`).

- add spaces before the opening parenthesis in if-statements and loops (`for(i=0` -> `for (i = 0`)

- add spaces after C-style casts (`(double)a` -> (double) a`)

- add some extra spaces for trailing comments (`a = 1; // comment` -> `a = 1;    // comment`)

- create a consistent "hanging brace" style (with functions, if statements and loops)

- add braces around single-line if statements

The various whitespace additions are to make the code (and comments)
hopefully more readable.


### Class, function and variable names


Class names follow PascalCase: each (sub)word is started with a
capital. This is done in a rather relaxed manner: `BBody` instead of
`BlackBody`, `Powerlaw` and `Cyclosyn` instead of `PowerLaw` and `CycloSyn`.

Function and variable names are snake_case: use underscores to
separate parts of the function. The underscore tends to read better
(more space) than with camelCase, where long names can become
cumbersome to read (`a_very_long_variable_name` vs
`aVeryLongVariableName`).


Header / include files
----------------------

Include header files only where they are needed: sometimes, they are
needed only in the cpp file, so don't include them in the related hpp
file. If both files need them, include them in both.

Be aware that the order of header files shouldn't matter. In fact,
clang-format will order the header files alphabetically.
