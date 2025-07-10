[TOC]

# Installation

The Kariba library can be installed using CMake. The installation has been tested against CMake version 3.26, 3.28 and 4.0.

## Prerequisites

The library has two prerequisites:

- The GNU Scientific Library, GSL. This can often be installed with help of a package manager.

- PyBind11. This can be installed with `python -m pip install
  pybind11`, and sometimes through a package manager. Either option
  should work; just ensure you are using at least version 2.13.

  If you like, you can install PyBind11 in a virtual environment. Just make
  sure that virtual environment is activated when you build the Kariba
  library.


## Download

First, download or clone the repository.

You can download a zip file from https://github.com/evertrol/bhjet (click the green '<> Code' button, then select "Download ZIP"), or
clone the repository: on the command line, in a suitable directory:

```
  git clone https://github.com/evertrol/BHJet.git

  cd BHJet
```

## Set up the installation


Once inside the local repository, take the following steps:

- Edit `cmake.config` and set the `CMAKE_INSTALL_PREFIX` variable to the base directory where you want the library to be installed. The actual library will be installed in the `lib` subdirectory, while the header files are installed in the `include/kariba` subdirectory (the directories are created if they do not exist).

  You can always change this prefix with the `--install-prefix` option on the fly later during the installation process.

  Note that you do not actually need to install the library to use it; it only needs to be build.

- Edit `cmake.config` and update the base path(s) to the GSL library. If GSL is installed through your system package manager, there is no need to set this. If it is installed in another way or in a non-system location, change this accordingly. For example, Homebrew may install GSL in `/opt/homebrew/Cellar/gsl/2.8` or similar.

  The include path and library path follow from the base directory, by appending `include` and `lib`. If these directories are different, change the `GSL_INCLUDE_DIR` and `GSL_LIBRARY_DIR` explicitly (Note that there should be a `gsl` subdirectory in the `include` directory, which should not be included in the `GSL_INCLUDE_DIR`).

- Check that PyBind11 for Python is installed. Running `python -m pybind11` on the command line should show a help message (instead of an error), and `python -m pybind11 --cmakedir` should print the directory where PyBind11 stores CMake configuration files. (This command is exactly what is being used during the installation to locate and use PyBind11.)


## Build the code

In the base BHJet directory, create a build directory; simply called it `build/`, and change to it:
```
mkdir build
cd build
```

Now, run CMake, pointing to the base directory:
```
cmake ..
```

This will configure the build process for your system.

Now, build the library
```
cmake --build .
```

If something goes wrong during the configuration or build process, you
can simply remove everything in this `build` directory, fix the
problem, and run the configuration and build step again.

The build should result in several library files, called libkariba.a
(the static library) and libkariba.so (the shared library).

The header files are in the `include/kariba` directory inside the
`build` directory.

For most projects, the static library is easier to use, in the sense
that this will result in an executable that includes all the functions
in the library, and does not need to link against the shared library.


If you want, you can now install the code, with
```
cmake --install .
```

This essentially only copies the files to a more generic directory. If
you like, you can change this directory from what is configuration in
the `config.cmake` file, with the `--install-prefix` option. For
example:

```
cmake --install --install-prefix=$HOME/sw .
```


## Compiling and running the test and examples

If you build everything from the root directory, the unit tests and the examples should already have been built. Otherwise, enter the respective directories, tests/ and examples/, and simply run `make`.

Once build, inside the respective directories, you can run all the unit tests with

```
./test_main
```

Hopefully all tests pass.

For the examples, inside their directory, run them with

```
./corona
./particles
./singlezone
```

The singlezone example produces some output to the terminal:

```
Physical quantities:
Average electron Lorenz factor: 8038.47
Equipartition Ue/UB: 698.369
Electron power: 1.80399e+43
Magnetic power: 2.58315e+40
Proton power: 4.12068e+42
Total power: 2.21864e+43 erg s^-1, 2.73063e-05 Eddington
```

All the examples also write a bunch of output files in the output/
directory. You can check the output files for correctness by comparing
them with those in the output_comparison directory.
