/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_CONTEXT_H
#define SQLTOAST_PARSER_CONTEXT_H

#include "sqltoast/sqltoast.h"

#include "parser/lexer.h"
#include "parser/token.h"

namespace sqltoast {

typedef struct parse_context {
    parse_result_t& result;
    parse_options_t& opts;
    lexer_t lexer;
    parse_context(parse_result_t& result, parse_options_t& opts, parse_input_t& subject) :
        result(result),
        opts(opts),
        lexer(subject)
    {}
} parse_context_t;

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_CONTEXT_H */
