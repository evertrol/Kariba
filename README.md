# Kariba

[![build-and-test](https://github.com/evertrol/Kariba/actions/workflows/ci.yml/badge.svg)](https://github.com/evertrol/Kariba/actions/workflows/ci.yml)

Kariba is a small C++ library to support jet models. In particular, it was built to support [BHJet](https://github.com/matteolucchini1/BHJet), a semi-analytical, multi-zone jet model designed for modelling steady-state SEDs of jets launched from accreting black holes.

# Installation

Once downloaded, you install Kariba by setting configurations in `make.config` at the root of the repository. In particular, set a path to your installation of the GNU Scientific Library (GSL), and whether you want to use OpenMP. You can also set an installation prefix, but this isn't necessary.

Once configured, you can simply run `make` to build the library, the unit tests and the examples. You can then run `./tests/test_main` to verify that everything was built correctly.

The library itself is available as a static library, `libkariba.a`, that you can include in any model you create.

You can install the library in a specific place, if you have set the prefix in `make.config` and then run `make install`.

Finally, an option exist to create a shared library as well. The library itself, however, is small enough that including it directly as a static library is probably more convenient.

For more details, see the installation instructions in the documentation.

# Developing & contributing

If you want to build your own model with help of the Kariba library, just build (and optionally install) the library as above. Please cite Lucchini et al. 2012, 2022, if you use the library (see section below).

If you find bugs or other issues in the library, or would like to suggest improvements, please file an issue. You can also file a pull request, though creating an issue first may help in clarifying the issue first, improving any fixes or features.

# Documentation

Documentation for Kariba can be found at the GitHub pages of this repository, https://kariba.github.io/Kariba .


# Acknowledging and citing Kariba or BHJet

Please cite Lucchini et al. 2021, 2022 if you find Kariba useful in your research. If you find BHJet useful, please also cite the original AGN jet article, Markoff et al. 2001, 2005. The bibtex entries can be found in the `citations.bib` file in this repository. Note that the `CITATION.cff` file only contains the Lucchini et al. 2021 entry.

# License

The software is licensed under the MIT license -- see the `LICENSE` file.
