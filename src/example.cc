// A demonstration of the sqltoast library API

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include "config.h"

#include "parse.h"

int main (int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stdout,"%s Version %d.%d\n",
            argv[0],
            sqltoast_VERSION_MAJOR,
            sqltoast_VERSION_MINOR);
        return 1;
    }
    sqltoast::parse_result_t res;
    const std::string s("CREATE DATABASE 'example';");
    sqltoast::parser_input_t subject(s.cbegin(), s.cend());

    res = sqltoast::parse(subject);

    if (res.code == sqltoast::SUCCESS) {
        fprintf(stdout, "Parsing successful.\n");
    } else {
        fprintf(stdout, "Parsing NOT successful.\n");
        for (auto err : res.errors) {
            fprintf(stdout, "Error: %s\n", err.data());
        }
    }
    return 0;
}
