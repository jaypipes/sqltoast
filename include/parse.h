/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSE_H
#define SQLTOAST_PARSE_H

#include <memory>
#include <stack>
#include <vector>

#include "sqltoast.h"
#include "ast.h"
#include "internal/symbols.h"

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
    symbol current_symbol;
    escape_mode current_escape;
    parse_position_t start_pos;
    parse_position_t end_pos;
    parse_cursor_t cursor;
    ast_t ast;
    parse_context(parse_result_t& result, parse_input_t& subject) :
        result(result),
        current_symbol(SOS),
        current_escape(ESCAPE_NONE),
        start_pos(subject.cbegin()),
        end_pos(subject.cend()),
        cursor(subject.begin())
    {}
} parse_context_t;

// Returns true if the parse context's current symbol is the supplied symbol
bool accept(parse_context_t& ctx, symbol s);

// Returns true if the parse context's current symbol is the supplied symbol
// and moves the context's cursor to the next symbol. If the current symbol is
// NOT what is expected, then sets the context's error message to indicate a
// syntax error.
bool expect(parse_context_t& ctx, symbol s);

// Moves the supplied parse context's cursor to the next symbol found in the
// context's input stream and sets the context's current symbol to the found
// symbol.
void next_symbol(parse_context_t& ctx);

// Moves the supplied parse context's cursor to the next symbol representing a
// single character terminal found in the context's input stream and sets the
// context's current symbol to the found symbol. Returns whether a
// single-character terminal symbol was found.
bool single_char_terminal(parse_context_t& ctx);

// Reads all content up to the next occurrence of the supplied token and
// returns a string with that content. Advances the cursor of the parse
// context to the position of the next occurrence of the supplied symbol.
const std::string consume_to_next(parse_context_t& ctx, symbol s);

bool comment(parse_context_t& ctx);

// Simply advances the parse context's cursor over any whitespace
void skip_ws(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSE_H */
