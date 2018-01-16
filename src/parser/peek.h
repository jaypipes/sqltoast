/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_PEEK_H
#define SQLTOAST_PARSER_PEEK_H

#include "context.h"

namespace sqltoast {

inline bool peek_char(parse_context_t& ctx, const char c) {
    return ((ctx.cursor != ctx.end_pos && (*(ctx.cursor+1) == c)));
}

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_PEEK_H */
