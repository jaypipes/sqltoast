# sqltoast - a parser for SQL written in C++

The `sqltoast` library is an Apache 2 licensed C++ library designed to be both
fast and fully thread-safe.

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

Each `sqltoast::statement_t` struct has its own "printer function" that allows
the user of `libsqltoast` to easily use `<<` operator to print the contents of
a `sqltoast::statement_t` to an `std::ostream`.

Examining the printer function for a subclass of `sqltoast::statement_t` will
allow you to understand the various attributes and sub-structs that comprise
the SQL statement.

For example, let's take a look at the printer function (in
[libsqltoast/src/statement.cc](libsqltoast/src/statement.cc) for the
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
