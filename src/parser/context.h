/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_CONTEXT_H
#define SQLTOAST_PARSER_CONTEXT_H

#include <memory>
#include <stack>
#include <vector>

#include "sqltoast.h"
#include "ast.h"
#include "token.h"

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

typedef struct parse_context {
    parse_result_t& result;
    std::stack<token_t> tokens;
    escape_mode current_escape;
    parse_position_t start_pos;
    parse_position_t end_pos;
    parse_cursor_t cursor;
    ast_t ast;
    parse_context(parse_result_t& result, parse_input_t& subject) :
        result(result),
        current_escape(ESCAPE_NONE),
        start_pos(subject.cbegin()),
        end_pos(subject.cend()),
        cursor(subject.begin())
    {}
} parse_context_t;

// Returns true if the parse context's current symbol is the supplied symbol
bool accept(parse_context_t& ctx, token_type_t tt);

// Returns true if the parse context's current symbol is the supplied symbol
// and moves the context's cursor to the next symbol. If the current symbol is
// NOT what is expected, then sets the context's error message to indicate a
// syntax error.
bool expect(parse_context_t& ctx, token_type_t tt);

// Moves the supplied parse context's cursor to the next symbol found in the
// context's input stream and sets the context's current symbol to the found
// symbol.
void next_symbol(parse_context_t& ctx);

// Simply advances the parse context's cursor over any whitespace
void skip_ws(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_CONTEXT_H */
