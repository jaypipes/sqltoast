/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "literal.h"
#include "token.h"

namespace sqltoast {

bool token_literal(parse_context_t& ctx) {
    lexer_t& lex = ctx.lexer;
    parse_cursor_t start = lex.cursor;
    symbol_t found_sym;
    bool found_sign = false; // set to true if + or - found

    char c = *lex.cursor;
    if (c == '+' || c == '-') {
        found_sign = true;
        c = *(++lex.cursor);
    }
    if (std::isdigit(c))
        goto try_numeric;
    switch (c) {
        case '.':
            goto try_numeric;
        default:
            goto not_found;
    }
try_numeric:
    // read to the next separator. if all characters are numbers, this is an
    // unsigned integer or decimal
    found_sym = SYMBOL_LITERAL_UNSIGNED_INTEGER;
    if (found_sign)
        found_sym = SYMBOL_LITERAL_SIGNED_INTEGER;
    {
        bool found_decimal = false;
        bool found_e = false;
        for (;;) {
            c = *lex.cursor++;
            if (lex.cursor == lex.end_pos) {
                // Make sure if we got a single . that we followed it with at
                // least one number...
                if (*(lex.cursor - 1) == '.')
                    goto not_found;
                if (found_e) {
                    // Make sure the exponent has at least one number
                    if (! std::isdigit(*(lex.cursor - 1)))
                        goto not_found;
                    found_sym = SYMBOL_LITERAL_APPROXIMATE_NUMBER;
                }
                goto push_literal;
            }
            if (std::isspace(c)) {
                if (found_e) {
                    // Make sure the exponent has at least one number
                    if (! std::isdigit(*(lex.cursor - 2)))
                        goto not_found;
                    found_sym = SYMBOL_LITERAL_APPROXIMATE_NUMBER;
                }
                goto push_literal;
            }
            if (std::isdigit(c))
                continue;
            switch (c) {
                case ',':
                case ')':
                case '(':
                case ';':
                    // Make sure if we got a single . that we followed it with
                    // at least one number...
                    if (*(lex.cursor - 2) == '.')
                        goto not_found;
                    if (found_e) {
                        // Make sure the exponent has at least one number
                        if (! std::isdigit(*(lex.cursor - 2)))
                            goto not_found;
                        found_sym = SYMBOL_LITERAL_APPROXIMATE_NUMBER;
                    }
                    goto push_literal;
                case '.':
                    // Make sure we have only one period...
                    if (found_decimal)
                        goto not_found;
                    found_decimal = true;
                    found_sym = SYMBOL_LITERAL_UNSIGNED_DECIMAL;
                    if (found_sign)
                        found_sym = SYMBOL_LITERAL_SIGNED_DECIMAL;
                    continue;
                case 'E':
                    // Make sure we haven't already found an "E", which would
                    // indicate we are expecting a signed integer as the
                    // exponent, not a decimal value
                    if (found_e)
                        goto not_found;
                    // Approximate (scientific-notation) numbers are designated with a
                    // <exact numeric literal>E<signed integer> grammar
                    found_e = true;
                    // Make sure we have found at least a digit before the 'E'
                    if (! std::isdigit(*(lex.cursor - 2)))
                        goto not_found;
                    continue;
                case '+':
                case '-':
                    // We should ONLY get here if there is an approximate number
                    // (e.g. 3.667E-10).
                    if (! found_e)
                        goto not_found;
                    continue;
                default:
                    goto not_found;
            }
        }
    }
push_literal:
    lex.cursor--;
    lex.set_token(
        found_sym,
        parse_position_t(start),
        parse_position_t(lex.cursor));
    return true;
not_found:
    lex.cursor = start; // rewind... we didn't find a literal
    return false;
}

} // namespace sqltoast
