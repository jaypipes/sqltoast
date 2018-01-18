/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_LEXER_H
#define SQLTOAST_PARSER_LEXER_H

#include "context.h"

namespace sqltoast {


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

#endif /* SQLTOAST_PARSER_LEXER_H */
