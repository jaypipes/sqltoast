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
