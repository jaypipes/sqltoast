/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "context.h"
#include "error.h"
#include "identifier.h"
#include "symbol.h"
#include "token.h"

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
bool token_identifier(parse_context_t& ctx) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = lex.cursor;
    escape_mode current_escape = ESCAPE_NONE;

    // Let's first look to see if we have the potential start of a delimited
    // identifier of some sort...
    switch (*lex.cursor) {
        case '\'':
            current_escape = ESCAPE_SINGLE_QUOTE;
            lex.cursor++;
            break;
        case '"':
            current_escape = ESCAPE_DOUBLE_QUOTE;
            lex.cursor++;
            break;
        case '`':
            current_escape = ESCAPE_TILDE;
            lex.cursor++;
            break;
        case 'U':
            // TODO(jaypipes): Check for PostgreSQL-style Unicode delimited
            // identifiers that look like U&"\0441\043B\043E\043D"
            break;
    }
    if (current_escape != ESCAPE_NONE)
        // handle delimited identifiers...
        return token_delimited_identifier(ctx, current_escape);

    // If we're not a delimited identifier, then consume all non-space characters
    // until the end of the parse subject or the next whitespace character
    while (! std::isspace(*lex.cursor)
            && lex.end_pos != lex.cursor
            && *lex.cursor != ';'
            && *lex.cursor != '('
            && *lex.cursor != ')'
            && *lex.cursor != ',')
        lex.cursor++;

    // if we went more than a single character, that's an
    // identifier...
    bool res = (start != lex.cursor);
    if (res) {
        lex.set_token(SYMBOL_IDENTIFIER, start, parse_position_t(lex.cursor));
    }
    return res;
}

bool token_delimited_identifier(parse_context_t& ctx, escape_mode current_escape) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = lex.cursor;
    char closer;
    switch (current_escape) {
        case ESCAPE_SINGLE_QUOTE:
            closer = '\'';
            break;
        case ESCAPE_DOUBLE_QUOTE:
        case ESCAPE_UNICODE_AMPERSAND:
            closer = '"';
            break;
        case ESCAPE_TILDE:
            closer = '`';
            break;
        case ESCAPE_NONE:
            return false;
    }
    char c;
    while (lex.cursor != lex.end_pos) {
        lex.cursor++;
        c = *lex.cursor;
        if (c == closer) {
            lex.set_token(SYMBOL_IDENTIFIER, start, parse_position_t(lex.cursor));
            return true;
        }
    }
    // We will get here if there was a start of a delimited escape sequence but we
    // never found the closing escape character(s). Set the parse context's
    // error to indicate the location that an error occurred.
    std::stringstream estr;
    estr << "In delimited identifier parsing, failed to find closing escape character ";
    if (closer == '\'') {
        estr << "\"\'\".\n";
    } else {
        estr << "\'" << closer << "\'.\n";
    }
    create_syntax_error_marker(ctx, estr, start);
    ctx.result.error.assign(estr.str());
    lex.error = ERR_NO_CLOSING_DELIMITER;
    return false;
}

} // namespace sqltoast
