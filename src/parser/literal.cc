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
        case '.':
            goto try_unsigned_numeric;
        default:
            goto not_found;
    }
try_unsigned_numeric:
    // read to the next separator. if all characters are numbers, this is an
    // unsigned integer or decimal
    found_sym = SYMBOL_LITERAL_UNSIGNED_INTEGER;
    {
        bool found_decimal = false;
        for (;;) {
            c = *ctx.cursor++;
            if (ctx.cursor == ctx.end_pos) {
                // Make sure if we got a single . that we followed it with at
                // least one number...
                if (*(ctx.cursor - 1) == '.')
                    goto not_found;
                goto push_literal;
            }
            if (std::isspace(c))
                goto push_literal;
            if (std::isdigit(c))
                continue;
            switch (c) {
                case ',':
                case ')':
                case '(':
                case ';':
                    // Make sure if we got a single . that we followed it with at
                    // least one number...
                    if (*(ctx.cursor - 2) == '.')
                        goto not_found;
                    goto push_literal;
                case '.':
                    // Make sure we have only one period...
                    if (found_decimal)
                        goto not_found;
                    found_decimal = true;
                    found_sym = SYMBOL_LITERAL_UNSIGNED_DECIMAL;
                    continue;
                default:
                    goto not_found;
            }
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
