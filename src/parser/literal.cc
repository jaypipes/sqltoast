/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "literal.h"
#include "token.h"

namespace sqltoast {

bool token_literal(parse_context_t& ctx) {
    parse_cursor_t start = ctx.cursor;
    symbol_t found_sym;
    char c = *ctx.cursor;
    if (std::isdigit(c))
        goto try_unsigned_numeric;
    switch (c) {
        default:
            goto not_found;
    }
try_unsigned_numeric:
    // read to the next separator. if all characters are numbers, this is an
    // unsigned numeric
    found_sym = SYMBOL_LITERAL_UNSIGNED_NUMERIC;
    for (;;) {
        c = *ctx.cursor++;
        if (ctx.cursor == ctx.end_pos)
            goto push_literal;
        if (std::isspace(c))
            goto push_literal;
        if (std::isdigit(c))
            continue;
        switch (c) {
            case ',':
            case ')':
            case '(':
            case ';':
                goto push_literal;
            default:
                goto not_found;
        }
    }
push_literal:
    ctx.cursor--;
    {
        token_t tok(
            TOKEN_TYPE_LITERAL,
            found_sym,
            parse_position_t(start),
            parse_position_t(ctx.cursor)
        );
        ctx.push_token(tok);
    }
    return true;
not_found:
    ctx.cursor = start; // rewind... we didn't find a literal
    return false;
}

} // namespace sqltoast
