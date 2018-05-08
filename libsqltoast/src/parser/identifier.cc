/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/sqltoast.h"

#include "parser/identifier.h"

namespace sqltoast {

//
// An identifier comes in a number of possible forms, and those forms can vary
// depending on the SQL dialect in use. A period is used to indicate an object
// boundary for an identifier. For instance, the string "t1.c1" would indicate
// that "c1" is a member of the object "t1". The string "db.t.c" would indicate
// that "c" is a member of "t" which is a member of the "db" object.
//
// In addition to dots to indicate object boundaries, individual identifier
// object names can be enclosed with quotes (or in the case of MySQL, backticks
// or quotes). These kinds of identifiers are commonly called "delimited
// identifiers" or "delimited identifiers". Finally, some database servers such as
// PostgreSQL use special qualifiers like 'U&"' to indicate a delimited identifier
// that uses Unicode-encoded characters in the identifier.
//
// Note that whitespace will have been skipped already so that the character
// pointed to by the parse context is guaranteed to be not whitespace.
tokenize_result_t token_identifier(
        parse_position_t cursor,
        const parse_position_t end) {
    parse_position_t start = cursor;
    escape_mode current_escape = ESCAPE_NONE;

    // Let's first look to see if we have the potential start of a delimited
    // identifier of some sort...
    switch (*cursor) {
        case '"':
            current_escape = ESCAPE_DOUBLE_QUOTE;
            cursor++;
            break;
        case '`':
            current_escape = ESCAPE_TILDE;
            cursor++;
            break;
        case 'U':
            // TODO(jaypipes): Check for PostgreSQL-style Unicode delimited
            // identifiers that look like U&"\0441\043B\043E\043D"
            break;
    }
    if (current_escape != ESCAPE_NONE)
        // handle delimited identifiers...
        return token_delimited_identifier(cursor, end, current_escape);

    // The first character of a non-delimited identifier must be a latin1 alpha
    if (! std::isalpha(*cursor++))
        return tokenize_result_t(TOKEN_NOT_FOUND);

    // If we're not a delimited identifier, then consume all non-space characters
    // until the end of the parse subject or the next whitespace character
    while (cursor != end && (std::isalnum(*cursor) || *cursor == '.' || *cursor == '_' || *cursor == '*'))
        cursor++;

    // if we went more than a single character, that's an identifier...
    if (start != cursor)
        return tokenize_result_t(SYMBOL_IDENTIFIER, start, cursor);
    return tokenize_result_t(TOKEN_NOT_FOUND);
}

tokenize_result_t token_delimited_identifier(
        parse_position_t cursor,
        const parse_position_t end,
        escape_mode current_escape) {
    parse_position_t start = cursor;
    char closer;
    switch (current_escape) {
        case ESCAPE_DOUBLE_QUOTE:
        case ESCAPE_UNICODE_AMPERSAND:
            closer = '"';
            break;
        case ESCAPE_TILDE:
            closer = '`';
            break;
        default:
            return tokenize_result_t(TOKEN_NOT_FOUND);
    }
    char c;
    while (cursor != end) {
        cursor++;
        c = *cursor;
        if (c == closer) {
            return tokenize_result_t(SYMBOL_IDENTIFIER, start, cursor);
        }
    }
    // We will get here if there was a start of a delimited escape sequence but we
    // never found the closing escape character(s). Set the parse context's
    // error to indicate the location that an error occurred.
    return tokenize_result_t(TOKEN_ERR_NO_CLOSING_DELIMITER, start, cursor);
}

} // namespace sqltoast
