/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_LEXER_H
#define SQLTOAST_PARSER_LEXER_H

#include <vector>

#include "context.h"
#include "symbol.h"

namespace sqltoast {

// Returns true if the parse context's current token is the supplied token
bool accept(parse_context_t& ctx, token_type_t tt);

// Returns true if the parse context's current token is the supplied token
// and moves the context's cursor to the next token. If the current token is
// NOT what is expected, then sets the context's error message to indicate a
// syntax error.
bool expect(parse_context_t& ctx, token_type_t tt);

// Attempts to find the next token in the parse context. If a token was found,
// returns true, else false. If true, the parse context's tokens stack will
// have had a token pushed onto it.
bool next_token(parse_context_t& ctx);

// Simply advances the parse context's cursor over any whitespace
void skip_ws(parse_context_t& ctx);

// Run through the subject string to parse, creating a stack of token_t's on
// the parse context.
void tokenize(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_LEXER_H */