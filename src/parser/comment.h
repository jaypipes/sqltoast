/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_COMMENT_H
#define SQLTOAST_PARSER_COMMENT_H

#include "context.h"

namespace sqltoast {

// Moves the supplied parse context's cursor to the next comment found in the
// context's input stream and sets the context's current symbol to the found
// comment symbol. Returns whether a comment was found.
bool parse_comment(parse_context_t& ctx);

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_COMMENT_H */
