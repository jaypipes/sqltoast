/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast/sqltoast.h"

#include "parser/comment.h"

namespace sqltoast {

// A comment in SQL follows the following EBNF form:
// <comment> ::= <simple comment> | <bracketed comment>
//
// <simple comment> ::= <simple comment introducer> [ <comment character> ... ] <newline>
//
// <simple comment introducer> ::= <minus sign> <minus sign> [ <minus sign> ... ]
//
// <bracketed comment> ::=
//     <bracketed comment introducer> <bracketed comment contents> <bracketed comment terminator>
//
// <bracketed comment introducer> ::= <slash> <asterisk>
//
// <bracketed comment terminator> ::= <asterisk> <slash>
//
// <bracketed comment contents> ::= [ { <comment character> | <separator> }... ]
//
// <comment character> ::= <nonquote character> | <quote>
//
// Simple comments are skipped over by the lexer completely. Bracketed
// comments, because they are sometimes used to embed dialect-specific
// triggers, are consumed as tokens.

tokenize_result_t token_comment(
        parse_position_t cursor,
        const parse_position_t end) {
    parse_position_t start = cursor;
    if (*cursor != '/')
        return tokenize_result_t(TOKEN_NOT_FOUND);

    cursor++;
    if (*cursor != '*')
        return tokenize_result_t(TOKEN_NOT_FOUND);

    // OK, we found the start of a comment. Run through the subject until we
    // find the closing */ marker
    do {
        cursor++;
        if (cursor == end || (cursor + 1) == end) {
            return tokenize_result_t(TOKEN_ERR_NO_CLOSING_DELIMITER, start, cursor);
        }
    } while (*cursor != '*' || *(cursor + 1) != '/');
    return tokenize_result_t(SYMBOL_COMMENT, start, cursor + 1);
}

} // namespace sqltoast
