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

#include "ast.h"
#include "ast_node.h"
#include "symbols.h"

namespace sqltoast {

// Possible escape mode for literals and identifiers
enum escape_mode {
    ESCAPE_NONE = 0,
    ESCAPE_SINGLE_QUOTE = 1,
    ESCAPE_DOUBLE_QUOTE = 2,
    ESCAPE_TILDE = 3,
    ESCAPE_UNICODE_AMPERSAND = 4
};

// Possible return codes from parsing, tokenizing, etc
enum parse_result_code {
    SUCCESS = 0,
    SYNTAX_ERROR = 1
};

typedef std::vector<char> parser_input_t;
typedef std::vector<char>::const_iterator parser_position_t;
typedef std::vector<char>::iterator parser_cursor_t;

typedef struct parse_result {
    parse_result_code code;
    parser_position_t position;
    std::vector<std::string> errors;
} parse_result_t;

typedef struct parser_context {
    symbol current_symbol;
    escape_mode current_escape;
    parser_position_t start_pos;
    parser_position_t end_pos;
    parser_cursor_t cursor;
    std::unique_ptr<ast_t> ast;
    std::vector<std::string> errors;
} parser_context_t;

// Returns true if the parser context's current symbol is the supplied symbol
bool accept(parser_context_t& ctx, symbol s);

// Returns true if the parser context's current symbol is the supplied symbol
// and moves the context's cursor to the next symbol. If the current symbol is
// NOT what is expected, then sets the context's error message to indicate a
// syntax error.
bool expect(parser_context_t& ctx, symbol s);

// Moves the supplied parser context's cursor to the next symbol found in the
// context's input stream and sets the context's current symbol to the found
// symbol.
void next_symbol(parser_context_t& ctx);

// Moves the supplied parser context's cursor to the next symbol representing a
// single character terminal found in the context's input stream and sets the
// context's current symbol to the found symbol. Returns whether a
// single-character terminal symbol was found.
bool single_char_terminal(parser_context_t& ctx);

// Reads all content up to the next occurrence of the supplied token and
// returns a string with that content. Advances the cursor of the parser
// context to the position of the next occurrence of the supplied symbol.
const std::string consume_to_next(parser_context_t& ctx, symbol s);

bool comment(parser_context_t& ctx);

// Simply advances the parser context's cursor over any whitespace
void skip_ws(parser_context_t& ctx);

parse_result_t parse(parser_input_t& subject);

} // namespace sqltoast

#endif /* SQLTOAST_PARSE_H */
