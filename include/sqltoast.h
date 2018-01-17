// DO NOT EDIT. Only edit the lnclude/sqltoast.h.in file, since sqltoast.h is
// generated during build.
#ifndef SQLTOAST_H
#define SQLTOAST_H

#define SQLTOAST_VERSION_MAJOR 0
#define SQLTOAST_VERSION_MINOR 1

#include <vector>
#include <string>

namespace sqltoast {

// Possible return codes from parsing
enum parse_result_code {
    PARSE_SUCCESS,
    PARSE_INPUT_ERROR,
    PARSE_SYNTAX_ERROR
};

typedef std::vector<char> parse_input_t;

typedef struct parse_result {
    parse_result_code code;
    std::string error;
} parse_result_t;

parse_result_t parse(parse_input_t& subject);

} // namespace sqltoast

#endif /* SQLTOAST_PARSE_H */
