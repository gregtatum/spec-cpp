# C++ Learning Spec

This is a test suite that encapsulates my C++ learning in a series of tests. The building process is done through a `makefile` so from the command line run `make` to build it, and then `./main` to run it. Or do both steps at the same time with `make test`. Individual tests can be run by setting the `TEST` environment variable.

```
make test TEST=features::pointers
```

## Requirements

The build process assumes that `clang++` is on the path with the ability to use the std library `c++1y`.

ICU4C was added as a library, and it must be added to `/usr/local`. This directory is specified in `makefile-icu.mk`. To install the ICU library on macOS:

```
git clone git@github.com:unicode-org/icu.git
cd icu/icu4c/source
./runConfigureICU MacOSX
```

## Compiling Assembly Examples

In order to study assembly, examples are stored in `/asm`. Run the following code to build an example. To create a new one, use the directory structure `/asm/{test}/main.cpp`. The make command will generate the assembly, and then they can be manually documented.

```
./make-asm simple-assignment
```

## Building compile_commands.json for editor integration

```
./make-db.sh
```

## Test output
