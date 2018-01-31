/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_CONTEXT_H
#define SQLTOAST_PARSER_CONTEXT_H

#include <iostream>
#include <cctype>

#include <memory>
#include <vector>

#include "sqltoast.h"

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

// Attempts to find the next token in the parse context. If a token was found,
// returns true, else false. If true, the parse context's tokens stack will
// have had a token pushed onto it.
token_t* next_token(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_CONTEXT_H */
