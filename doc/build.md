# Repository layout

This source repository contains two directories with C++ code: the
[libsqltoast](../libsqltoast) directory contains the source code and headers
for the `sqltoast` library; the [sqltoaster](../sqltoaster) directory contains
source files for an example program that utilizes the `sqltoast` library.

Finally, there is a [tests](../tests) directory which contains various test
files and test infrastructure.

# Build instructions

`sqltoast` uses [cmake](https://cmake.org) as its primary build toolchain.

**NOTE**: If you add a new source or header file or dependency, you will need
to edit the `CMakeLists.txt` file in either the [libsqltoast](../libsqltoast)
directory or the [sqltoaster](../sqltoaster) directory.

To build the `sqltoast` library and `sqltoaster` example executable, first,
create a directory to use for building:

```
BUILD_DIR=""$(pwd)/_build"
mkdir -p $BUILD_DIR
```

Then change directory into your build directory and run `cmake`:

```bash
cd $BUILD_DIR
cmake ..
```

this will regenerate the `Makefile`s as needed and you may then build the
library and `sqltoaster` binary with a simple:

```bash
make
```

# Running tests

After building the library and `sqltoaster` binary, you can run tests by
changing into the [tests/grammar](../tests/grammar) directory and executing the
Python test runner, like so:

```
cd tests/grammar
python runner.py run
```

which will run all the grammar tests and output something like this:

```
jaypipes@uberbox:~/src/github.com/jaypipes/sqltoast$ cd tests/grammar/
jaypipes@uberbox:~/src/github.com/jaypipes/sqltoast/tests/grammar$ python runner.py run
Running ansi-92/alter-table ... OK
Running ansi-92/column-definitions ... OK
Running ansi-92/datetime-expressions ... OK
Running ansi-92/delete ... OK
Running ansi-92/identifiers ... OK
Running ansi-92/insert-default-values ... OK
Running ansi-92/insert-values ... OK
Running ansi-92/interval-expressions ... OK
Running ansi-92/numeric-expressions ... OK
Running ansi-92/predicates ... OK
Running ansi-92/string-expressions ... OK
Running ansi-92/table-references ... OK
Running ansi-92/update ... OK
```
