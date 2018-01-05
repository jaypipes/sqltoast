# Build instructions

`sqltoast` uses [cmake](https://cmake.org) as its primary build toolchain. If
you add a new source or header file or dependency, you will need to edit the
top-level `CMakeLists.txt` file, change directory into the `build/` directory
and run:

```bash
cmake ../
```

this will regenerate the `Makefile` as needed and you may then build `sqltoast`
with a simple:

```bash
make
```

from the `build/` directory.
