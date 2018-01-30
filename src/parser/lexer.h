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

// Attempts to find the next token in the parse context. If a token was found,
// returns true, else false. If true, the parse context's tokens stack will
// have had a token pushed onto it.
bool next_token(parse_context_t& ctx);

// Run through the subject string to parse, creating a stack of token_t's on
// the parse context.
void tokenize(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_LEXER_H */
