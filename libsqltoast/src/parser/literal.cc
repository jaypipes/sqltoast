/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/sqltoast.h"

#include "parser/literal.h"

namespace sqltoast {

// cursor is pointing at a non-space, non-\0 char
tokenize_result_t token_literal(
        parse_position_t cursor,
        const parse_position_t end) {
    switch (*cursor) {
        case '+':
        case '-':
            if (! std::isdigit(*(cursor + 1)))
                return tokenize_result_t(TOKEN_NOT_FOUND);
            return token_numeric_literal(cursor, end, true);
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return token_numeric_literal(cursor, end, false);
        case '\'':
            return token_character_string_literal(cursor, end,
                    SYMBOL_LITERAL_CHARACTER_STRING);
        case 'N':
            if (((cursor + 1) == end) || *(cursor + 1) != '\'')
                return tokenize_result_t(TOKEN_NOT_FOUND);
            ++cursor;
            return token_character_string_literal(cursor, end,
                    SYMBOL_LITERAL_NATIONAL_CHARACTER_STRING);
        case 'B':
            if (((cursor + 1) == end) || *(cursor + 1) != '\'')
                return tokenize_result_t(TOKEN_NOT_FOUND);
            ++cursor;
            return token_bit_string_literal(cursor, end);
        case 'X':
            if (((cursor + 1) == end) || *(cursor + 1) != '\'')
                return tokenize_result_t(TOKEN_NOT_FOUND);
            ++cursor;
            return token_hex_string_literal(cursor, end);
        default:
            return tokenize_result_t(TOKEN_NOT_FOUND);
    }
}

// cursor is pointing at a digit if found_sign is false, or either the + or -
// char if found_sign is true
tokenize_result_t token_numeric_literal(
        parse_position_t cursor,
        const parse_position_t end,
        bool found_sign) {
    parse_position_t start = cursor;
    symbol_t found_sym;
    bool found_decimal = false;
    bool found_e = false;
    if (found_sign)
        ++cursor;
    char c = *cursor;
    // read to the next separator. if all characters are numbers, this is an
    // unsigned integer or decimal
    found_sym = SYMBOL_LITERAL_UNSIGNED_INTEGER;
    if (found_sign)
        found_sym = SYMBOL_LITERAL_SIGNED_INTEGER;
    while (cursor != end) {
        if (std::isspace(*cursor)) {
            if (found_e) {
                // Make sure the exponent has at least one number
                if (! std::isdigit(*(cursor - 1)))
                    goto not_found;
                found_sym = SYMBOL_LITERAL_APPROXIMATE_NUMBER;
            }
            goto push_literal;
        }
        c = *cursor;
        if (std::isdigit(c)) {
            cursor++;
            continue;
        }
        switch (c) {
            case ',':
            case ')':
            case '(':
            case ';':
                // Make sure if we got a single . that we followed it with
                // at least one number...
                if (found_decimal && *(cursor - 1) == '.')
                    goto not_found;
                if (found_e) {
                    // Make sure the exponent has at least one number
                    if (! std::isdigit(*(cursor - 1)))
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
                cursor++;
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
                cursor++;
                continue;
            case '+':
            case '-':
                // We should ONLY get here if there is an approximate number
                // (e.g. 3.667E-10).
                if (! found_e)
                    goto not_found;
                cursor++;
                continue;
            default:
                goto not_found;
        }
    }
    // We get here if we hit the EOS but had parsed at least a number.  Make
    // sure if we got a single . that we followed it with at least one
    // number...
    {
        char last_char = *(cursor - 1);
        if (last_char == '.')
            goto not_found;
        // Make sure the exponent has at least one number
        if (found_e) {
            if (! std::isdigit(last_char))
                goto not_found;
            found_sym = SYMBOL_LITERAL_APPROXIMATE_NUMBER;
        }
        goto push_literal;
    }
push_literal:
    return tokenize_result_t(found_sym, start, cursor);
not_found:
    return tokenize_result_t(TOKEN_NOT_FOUND);
}

// cursor is pointing at the \' char
tokenize_result_t token_character_string_literal(
        parse_position_t cursor,
        const parse_position_t end,
        symbol_t literal_sym) {
    parse_position_t start = cursor;
    char c = *cursor++;
    char last_c = c;
    while (cursor != end) {
        c = *cursor++;
        if (c == '\'' && last_c != '\\') {
            last_c = c;
            break;
        }
        last_c = c;
    }
    if (last_c == '\'' && (cursor - start > 1))
        goto push_literal;
    return tokenize_result_t(TOKEN_NOT_FOUND);
push_literal:
    return tokenize_result_t(literal_sym, start, cursor);
}

// cursor is pointing at the \' char
tokenize_result_t token_bit_string_literal(
        parse_position_t cursor,
        const parse_position_t end) {
    parse_position_t start = cursor;
    char c = *cursor++;
    char last_c = c;
    while (cursor != end) {
        c = *cursor;
        if (std::isspace(c))
            break;
        if (c != '0' && c != '1' && c != '\'')
            return tokenize_result_t(TOKEN_NOT_FOUND);
        last_c = c;
        ++cursor;
    }
    if (last_c == '\'' && (cursor - start > 1))
        goto push_literal;
    return tokenize_result_t(TOKEN_NOT_FOUND);
push_literal:
    return tokenize_result_t(SYMBOL_LITERAL_BIT_STRING, start, cursor);
}

// cursor is pointing at the \' char
tokenize_result_t token_hex_string_literal(
        parse_position_t cursor,
        const parse_position_t end) {
    parse_position_t start = cursor;
    char c = *cursor++;
    char last_c = c;
    while (cursor != end) {
        c = *cursor;
        if (std::isspace(c))
            break;
        if (! std::isxdigit(c) && c != '\'')
            return tokenize_result_t(TOKEN_NOT_FOUND);
        last_c = c;
        ++cursor;
    }
    if (last_c == '\'' && (cursor - start > 1))
        goto push_literal;
    return tokenize_result_t(TOKEN_NOT_FOUND);
push_literal:
    return tokenize_result_t(SYMBOL_LITERAL_HEX_STRING, start, cursor);
}

} // namespace sqltoast
