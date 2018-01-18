/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "comment.h"
#include "peek.h"
#include "symbol.h"
#include "token.h"

namespace sqltoast {

// A comment in SQL is a slash followed by an asterisk, then any characters
// (include newline) until an asterisk followed by a slash.
bool token_comment(parse_context_t& ctx) {
    if (! peek_char(ctx, '/'))
        return false;

    parse_cursor_t start = ctx.cursor;

    ctx.cursor++;
    if (! peek_char(ctx, '*')) {
        ctx.cursor = start;
        return false;
    }
    ctx.cursor++;

    token_t tok(TOKEN_TYPE_COMMENT, SYMBOL_COMMENT, parse_position_t(ctx.cursor), ctx.end_pos);
    ctx.tokens.emplace_back(tok);
    return true;
}

} // namespace sqltoast
