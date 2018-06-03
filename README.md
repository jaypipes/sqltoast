# sqltoast - a parser for SQL written in C++ [![Build Status](https://travis-ci.org/jaypipes/sqltoast.svg?branch=master)](https://travis-ci.org/jaypipes/sqltoast)

The `sqltoast` library is an Apache 2 licensed C++ library designed for both
fast and fully thread-safe parsing of SQL into a collection of structs that
represent the abstract syntax tree (thus SQL to AST, or "sqltoast").

## Overview

There is one primary function that the library publishes: ``sqltoast::parse()``.

This function comes in two flavors; one which accepts just the string to be
parsed and one which accepts the string to be parsed along with a struct
representing parsing options and instructions for sqltoast.

The return value of ``sqltoast::parse()`` is a reference to a struct
`sqltoast::parse_result_t` that describes the results of the parse:

```c++

#include <iostream>

#include <sqltoast/sqltoast.h>

int main(int argc, char *argv[]) {
    std::string input;
    if (argc == 2) {
        input.assign(args[1]);
    } else
        std::cout << "Supply a SQL string to parse." << std::endl;
        return 1;
    }
    sqltoast::parse_result_t res;
    sqltoast::parse_input_t subject(input.cbegin(), input.cend());

    res = sqltoast::parse(subject);
    if (res.code == sqltoast::PARSE_OK)
        std::cout << "Success!" << std::endl;
    else if (res.code == sqltoast::PARSE_SYNTAX_ERROR)
        std::cout << "Syntax error: " << res.error << std::endl;
}
```

An important attribute of the `sqltoast::parse_result_t` struct is the
`statements` field, which is of type
`std::vector<std::unique_ptr<sqltoast::statement_t>>`. For each valid SQL
statement parsed by `sqltoast::parse()`, there will be a
`std::unique_ptr<sqltoast::statement_t>` subclassed struct in the `statements`
field.

The [example program](sqltoaster/main.cc) included in the
[sqltoaster/](sqltoaster/) directory can be a good guide to lean about the
collection of `sqltoast::statement_t` structs.

The example program contains "printer functions" for each type of
`sqltoast::statement_t` struct that is part of the `sqltoast` library. The
example program outputs in a variety of formats, including YAML, so you can,
for example, see a YAML document that describes the parsed
`sqltoast::statement_t` type. Reading through the printer functions can give
you a good idea of the structure of these `sqltoast::statement_t` types and how
to utilize the parsed information.

After building the `sqltoaster` binary, let's ask it to parse a `SELECT`
statement and output the parsed information as a YAML document.

```
sqltoaster --yaml "SELECT a FROM t1"
statements:
- type: SELECT
  selected_columns:
    - column-reference[a]
  referenced_tables:
    - t1
(took 15790 nanoseconds)
```

By examining the `sqltoaster::print::to_yaml()` function in the `sqltoaster`
program, we can see how to read information about a particular
`sqltoast::statement_t` struct that is contained in the
`sqltoast::parse_result_t` struct's `statements` attribute.

**NOTE**: I've trimmed the code below and slightly modified it (removing some
indenting-related fluff) for brevity.

First, let's take a look at the `sqltoaster::print::to_yaml()` function that
receives a `sqltoast::statement_t` object. This is our "top-level" YAML-output
function for SQL statements.

```c++
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::statement_t& stmt) {
    switch (stmt.type) {
        // ...
        case sqltoast::STATEMENT_TYPE_SELECT:
            out << "type: SELECT";
            {
                const sqltoast::select_statement_t& sub =
                    static_cast<const sqltoast::select_statement_t&>(stmt);
                to_yaml(ptr, out, sub);
            }
            break;
        // ...
    }
}
```

The above demonstrates an important pattern deployed in the `sqltoast` library
and a fundamental technique used by the example `sqltoaster` program to sift
through the components of the parse tree.

For performance reasons, `sqltoast` is built with no run-time type
identification (RTTI). All type identification must be handled at compile time,
and due to this, users of the `sqltoast` library (in this case, the example
`sqltoaster` program) use a "type switch" approach to determine what class of
statement the supplied `sqltoast::statement_t` reference is and then use a
`static_cast<>` to convert the `sqltoast::statement_t&` into the appropriate
derived type.

Once you've `static_cast<>`'d to a reference of the derived type, you would
operate on that derived type reference, as shown in this code that examines the
elements of the `sqltoast::select_statement_t` sub-type and outputs those
elements into the YAML document.

```c++
void to_yaml(printer_t& ptr, std::ostream& out, const sqltoast::select_statement_t& stmt) {
    if (stmt.distinct)
       out << "distinct: true";
    out << "selected_columns:";
    for (const sqltoast::derived_column_t& dc : stmt.selected_columns)
        out << "- " << dc;
    out << "referenced_tables:";
    for (const std::unique_ptr<sqltoast::table_reference_t>& tr : stmt.referenced_tables)
        out << "- " << *tr;
    if (stmt.where_condition)
        out << "where:" << *stmt.where_condition;
    if (! stmt.group_by_columns.empty()) {
        out << "group_by:";
        for (const sqltoast::grouping_column_reference_t& gcr : stmt.group_by_columns)
            out << "- " << gcr;
    }
    if (stmt.having_condition)
        out << "having:" << *stmt.having_condition;
}
```

## Repository layout

This source repository contains two directories with C++ code: the
[libsqltoast](../libsqltoast) directory contains the source code and headers
for the `sqltoast` library; the [sqltoaster](../sqltoaster) directory contains
source files for an example program that utilizes the `sqltoast` library.

Finally, there is a [tests](../tests) directory which contains various test
files and test infrastructure.

## Build instructions

`sqltoast` uses [cmake](https://cmake.org) as its primary build toolchain.

**NOTE**: If you add a new source or header file or dependency, you will need
to edit the `CMakeLists.txt` file in either the [libsqltoast](../libsqltoast)
directory or the [sqltoaster](../sqltoaster) directory.

To build the `sqltoast` library and `sqltoaster` example executable, first,
create a directory to use for building:

```
BUILD_DIR="$(pwd)/_build"
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

## Running tests

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
Running ansi-92/case-expressions ... OK
Running ansi-92/column-definitions ... OK
Running ansi-92/create-schema ... OK
Running ansi-92/create-table ... OK
Running ansi-92/create-view ... OK
Running ansi-92/datetime-expressions ... OK
Running ansi-92/delete ... OK
Running ansi-92/drop-schema ... OK
Running ansi-92/drop-table ... OK
Running ansi-92/drop-view ... OK
Running ansi-92/grant ... OK
Running ansi-92/identifiers ... OK
Running ansi-92/joins ... OK
Running ansi-92/insert-select ... OK
Running ansi-92/insert-values ... OK
Running ansi-92/interval-expressions ... OK
Running ansi-92/numeric-expressions ... OK
Running ansi-92/predicates ... OK
Running ansi-92/row-value-constructors ... OK
Running ansi-92/string-expressions ... OK
Running ansi-92/table-references ... OK
Running ansi-92/update ... OK
```
