# C++ Learning Spec

This is a test suite that encapsulates my C++ learning in a series of tests. The building process is done through a `makefile` so from the command line run `make` to build it, and then `./main` to run it. Or do both steps at the same time with `make test`. Individual tests can be run by setting the `TEST` environment variable.

```
make test TEST=features::pointers
```

## Requirements

The build process assumes that `clang++` is on the path with the ability to use the std library `c++1y`.

## Test output
