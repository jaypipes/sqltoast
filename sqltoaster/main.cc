/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <string.h>

#include <iostream>

#include <sqltoast/sqltoast.h>

#include "measure.h"
#include "printer.h"

struct parser {
    sqltoast::parse_options_t opts;
    sqltoast::parse_input_t subject;
    sqltoast::parse_result_t res;
    parser(sqltoast::parse_options_t& opts, const std::string &input) :
        opts(opts),
        subject(input.cbegin(), input.cend())
    {}
    void operator()() {
        res = sqltoast::parse(subject, opts);
    }
};

void usage(const char* prg_name) {
    std::cout << "Usage: " << prg_name <<
        " [--disable-timer] [--yaml] <SQL>" << std::endl;
    std::cout << " using libsqltoast version " <<
        SQLTOAST_VERSION_MAJOR << '.' <<
        SQLTOAST_VERSION_MINOR << std::endl;
}

int main (int argc, char *argv[])
{
    std::string input;
    bool disable_timer = false;
    bool use_yaml = false;

    for (int x = 1; x < argc; x++) {
        if (strcmp(argv[x], "--disable-timer") == 0) {
            disable_timer = true;
            continue;
        }
        if (strcmp(argv[x], "--yaml") == 0) {
            use_yaml = true;
            continue;
        }
        input.assign(argv[x]);
        break;
    }
    if (input.empty()) {
        usage(argv[0]);
        return 1;
    }

    sqltoast::parse_options_t opts = {sqltoast::SQL_DIALECT_ANSI_1992, false};
    parser p(opts, input);

    auto dur = measure<std::chrono::nanoseconds>::execution(p);
    sqltoaster::printer ptr(p.res, std::cout);
    if (use_yaml)
        ptr.output_format = sqltoaster::OUTPUT_FORMAT_YAML;
    if (p.res.code == sqltoast::PARSE_OK)
        std::cout << ptr << std::endl;
    else if (p.res.code == sqltoast::PARSE_INPUT_ERROR)
        std::cout << "Input error: " << p.res.error << std::endl;
    else {
        std::cout << "Syntax error." << std::endl;
        std::cout << p.res.error << std::endl;
    }
    if (! disable_timer)
        std::cout << "(took " << dur << " nanoseconds)" << std::endl;
    return 0;
}
