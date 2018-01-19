/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_IDENTIFIER_H
#define SQLTOAST_IDENTIFIER_H

#include <string>

#include "context.h"

namespace sqltoast {

// Returns true if an identifier (of any kind) can be parsed from the parse
// context's cursor position
bool token_identifier(parse_context_t& ctx);

// Returns true if a delimited identifier can be parsed from the parse
// context's cursor position.
bool token_delimited_identifier(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_IDENTIFIER_H */
