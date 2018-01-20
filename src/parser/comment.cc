/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "parser/error.h"
#include "parser/comment.h"
#include "parser/peek.h"
#include "parser/symbol.h"
#include "parser/token.h"

namespace sqltoast {

// A comment in SQL is a slash followed by an asterisk, then any characters
// (include newline) until an asterisk followed by a slash.
bool token_comment(parse_context_t& ctx) {
    if (! peek_char(ctx, '/'))
        return false;

    ctx.cursor++;
    if (! peek_char(ctx, '*')) {
        ctx.cursor--; // rewind
        return false;
    }

    parse_position_t start = ctx.cursor - 2;

    // OK, we found the start of a comment. Run through the subject until we
    // find the closing */ marker
    do {
        ctx.cursor++;
        if (ctx.cursor == ctx.end_pos || (ctx.cursor + 1) == ctx.end_pos) {
            goto err_no_end_marker;
        }
    } while (*ctx.cursor != '*' || *(ctx.cursor + 1) != '/');
    goto create_token;

    create_token:
    {
        ctx.cursor += 2;
        token_t tok(TOKEN_TYPE_COMMENT, SYMBOL_COMMENT, start, parse_position_t(ctx.cursor));
        ctx.push_token(tok);
        return true;
    }

    err_no_end_marker:
    {
        std::stringstream estr;
        estr << "Expected closing */ comment marker but found EOS" << std::endl;
        create_syntax_error_marker(ctx, estr, ctx.end_pos);
        return false;
    }
}

} // namespace sqltoast
