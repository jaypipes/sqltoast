/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <stdio.h>
#include <stdlib.h>

#include "internal/identifier.h"
#include "parse.h"

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
// identifiers" or "quoted identifiers". Finally, some database servers such as
// PostgreSQL use special qualifiers like 'U&"' to indicate a quoted identifier
// that uses Unicode-encoded characters in the identifier.
//
// Note that whitespace will have been skipped already so that the character
// pointed to by the parser context is guaranteed to be not whitespace.
bool parse_identifier(parser_context_t& ctx) {
    parser_position_t start = ctx.cursor;

    // Let's first look to see if we have the potential start of a quoted
    // identifier of some sort...
    switch (*ctx.cursor) {
        case '\'':
            ctx.current_escape = ESCAPE_SINGLE_QUOTE;
            ctx.cursor++;
            break;
        case '"':
            ctx.current_escape = ESCAPE_DOUBLE_QUOTE;
            ctx.cursor++;
            break;
        case '`':
            ctx.current_escape = ESCAPE_TILDE;
            ctx.cursor++;
            break;
        case 'U':
            // TODO(jaypipes): Check for PostgreSQL-style Unicode quoted
            // identifiers that look like U&"\0441\043B\043E\043D"
            break;
    }
    if (ctx.current_escape != ESCAPE_NONE)
        // handle quoted identifiers...
        return parse_quoted_identifier(ctx);

    // If we're not a quoted identifier, then consume all non-space characters
    // until the end of the parse subject or the next whitespace character
    while (! std::isspace(*ctx.cursor) && *ctx.cursor != ';' && ctx.end_pos != ctx.cursor) {
        ctx.cursor++;
    }
    // if we went more than a single character, that's an
    // identifier...
    bool res = (start != ctx.cursor);
    if (res) {
        ctx.current_symbol = IDENTIFIER;
        db_identifier_t id(start, ctx.cursor);
        // TODO(jaypipes): tack identifier into the AST
        fprintf(stdout, "found identifier: \"%s\"\n", id.name.data());
    }
    return res;
}

bool parse_quoted_identifier(parser_context_t& ctx) {
    parser_position_t start = ctx.cursor;
    char closer;
    switch (ctx.current_escape) {
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
    while (ctx.cursor != ctx.end_pos) {
        ctx.cursor++;
        c = *ctx.cursor;
        if (c == closer) {
            ctx.current_escape = ESCAPE_NONE;
            ctx.current_symbol = IDENTIFIER;
            db_identifier_t id(start, ctx.cursor);
            // TODO(jaypipes): tack identifier into the AST
            fprintf(stdout, "found identifier: \"%s\"\n", id.name.data());
            return true;
        }
    }
    // We will get here if there was a start of a quoted escape sequence but we
    // never found the closing escape character(s). Set the parser context's
    // error to indicate this is the location that an error occurred.
    char estr[200];
    std::string original(ctx.start_pos, ctx.end_pos);
    std::string location(original);
    auto start_pos = start - ctx.start_pos;
    for (auto cur = location.begin(); cur != location.end(); cur++) {
        auto loc_pos = cur - location.begin();
        if (loc_pos < start_pos) {
            *cur = ' ';
        } else {
            *cur = '^';
        }
    }

    sprintf(estr,
            "In quoted identifier parsing, failed to find closing escape character '%c'.\n"
            "%s\n%s", closer, original.data(), location.data());
    std::string e(estr);
    ctx.errors.push_back(e);
    return false;
}

} // namespace sqltoast
