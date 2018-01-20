/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_CONTEXT_H
#define SQLTOAST_PARSER_CONTEXT_H

#include <deque>
#include <memory>
#include <vector>

#include "sqltoast.h"

#include "parser/token.h"

namespace sqltoast {

// Possible escape mode for literals and identifiers
enum escape_mode {
    ESCAPE_NONE = 0,
    ESCAPE_SINGLE_QUOTE = 1,
    ESCAPE_DOUBLE_QUOTE = 2,
    ESCAPE_TILDE = 3,
    ESCAPE_UNICODE_AMPERSAND = 4
};

typedef std::vector<char>::const_iterator parse_position_t;
typedef std::vector<char>::iterator parse_cursor_t;
typedef std::deque<token_t> tokens_t;

typedef struct parse_context {
    parse_result_t& result;
    sql_dialect_t sql_dialect;
    escape_mode current_escape;
    parse_position_t start_pos;
    parse_position_t end_pos;
    parse_cursor_t cursor;
    tokens_t tokens;
    parse_context(parse_result_t& result, parse_input_t& subject) :
        result(result),
        sql_dialect(SQL_DIALECT_ANSI_2003),
        current_escape(ESCAPE_NONE),
        start_pos(subject.cbegin()),
        end_pos(subject.cend()),
        cursor(subject.begin())
    {}
    inline void push_token(token_t& tok) {
        tokens.emplace_back(tok);
    }
} parse_context_t;

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_CONTEXT_H */
