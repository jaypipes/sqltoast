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

One of the most important attributes of the `sqltoast::parse_result_t` struct
is the `statements` field, which is of type
`std::vector<std::unique_ptr<sqltoast::statement_t>>`. For each valid SQL
statement parsed by `sqltoast::parse()`, there will be a
`std::unique_ptr<sqltoast::statement_t>` subclassed struct in the `statements`
field.

The [example program](sqltoaster/main.cc) included in the
[sqltoaster/](sqltoaster/) directory can show you one way of interacting with
this important collection of structs:

```c++
    unsigned int x = 0;
    for (auto stmt_ptr_it = p.res.statements.cbegin();
            stmt_ptr_it != p.res.statements.cend();
            stmt_ptr_it++) {
        cout << "statements[" << x++ << "]:" << endl;
        cout << "  " << *(*stmt_ptr_it) << endl;
    }
```

The example program contains "printer functions" for each of the important
structures that are part of the `sqltoast` library.

These printer functions are merely operator `<<` overloads that accept a
`std::ostream` and one of the `sqltoast` library structures.

Examining the printer function for a subclass of `sqltoast::statement_t` will
help you understand the various attributes and sub-structs that comprise the
SQL statement.

For example, let's take a look at the printer function (in
[sqltoaster/src/statement.cc](sqltoast/src/statement.cc) for the
`sqltoast::select_t` statement subclass struct:

```c++
std::ostream& operator<< (std::ostream& out, const select_statement_t& stmt) {
    out << "<statement: SELECT";
    if (stmt.distinct)
       out << std::endl << "   distinct: true";
    out << std::endl << "   selected columns:";
    size_t x = 0;
    for (const derived_column_t& dc : stmt.selected_columns) {
        out << std::endl << "     " << x++ << ": " << dc;
    }
    out << std::endl << "   referenced tables:";
    x = 0;
    for (const std::unique_ptr<table_reference_t>& tr : stmt.referenced_tables) {
        out << std::endl << "     " << x++ << ": " << *tr;
    }
    if (stmt.where_condition) {
        out << std::endl << "   where:" << std::endl << "     ";
        out << *stmt.where_condition;
    }
    if (! stmt.group_by_columns.empty()) {
        out << std::endl << "   group by:";
        x = 0;
        for (const grouping_column_reference_t& gcr : stmt.group_by_columns) {
            out << std::endl << "     " << x++ << ": " << gcr;
        }
    }
    if (stmt.having_condition) {
        out << std::endl << "   having:" << std::endl << "     ";
        out << *stmt.having_condition;
    }
    out << ">" << std::endl;

    return out;
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
Running ansi-92/datetime-expressions ... OK
Running ansi-92/delete ... OK
Running ansi-92/drop-view ... OK
Running ansi-92/grant ... OK
Running ansi-92/identifiers ... OK
Running ansi-92/joins ... OK
Running ansi-92/insert-default-values ... OK
Running ansi-92/insert-values ... OK
Running ansi-92/interval-expressions ... OK
Running ansi-92/numeric-expressions ... OK
Running ansi-92/predicates ... OK
Running ansi-92/row-value-constructors ... OK
Running ansi-92/string-expressions ... OK
Running ansi-92/table-references ... OK
Running ansi-92/update ... OK
```
