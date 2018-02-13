/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/literal.h"

namespace sqltoast {

tokenize_result_t token_literal(
        parse_position_t cursor,
        const parse_position_t end) {
    parse_position_t start = cursor;
    symbol_t found_sym;
    bool found_sign = false; // set to true if + or - found

    char c = *cursor;
    if (c == '+' || c == '-') {
        found_sign = true;
        c = *(++cursor);
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
            c = *cursor++;
            if (cursor == end) {
                // Make sure if we got a single . that we followed it with at
                // least one number...
                if (*(cursor - 1) == '.')
                    goto not_found;
                if (found_e) {
                    // Make sure the exponent has at least one number
                    if (! std::isdigit(*(cursor - 1)))
                        goto not_found;
                    found_sym = SYMBOL_LITERAL_APPROXIMATE_NUMBER;
                }
                goto push_literal;
            }
            if (std::isspace(c)) {
                if (found_e) {
                    // Make sure the exponent has at least one number
                    if (! std::isdigit(*(cursor - 2)))
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
                    if (found_decimal && *(cursor - 2) == '.')
                        goto not_found;
                    if (found_e) {
                        // Make sure the exponent has at least one number
                        if (! std::isdigit(*(cursor - 2)))
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
                    if (! std::isdigit(*(cursor - 2)))
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
    return tokenize_result_t(found_sym, start, cursor);
not_found:
    return tokenize_result_t(TOKEN_NOT_FOUND);
}

} // namespace sqltoast
