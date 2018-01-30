/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "parser/error.h"
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

bool token_comment(parse_context_t& ctx) {
    return token_simple_comment(ctx) | token_bracketed_comment(ctx);
}

bool token_simple_comment(parse_context_t& ctx) {
    lexer_t& lex = ctx.lexer;
    if (! lex.peek_char('-'))
        return false;

    lex.cursor++;
    if (! lex.peek_char('-')) {
        lex.cursor--; // rewind
        return false;
    }

    // The comment content is from the cursor until we find a newline of EOS
    do {
        lex.cursor++;
    } while (lex.cursor != lex.end_pos && *lex.cursor != '\n');
    return true;
}

bool token_bracketed_comment(parse_context_t& ctx) {
    lexer_t& lex = ctx.lexer;
    if (! lex.peek_char('/'))
        return false;

    lex.cursor++;
    if (! lex.peek_char('*')) {
        lex.cursor--; // rewind
        return false;
    }

    // OK, we found the start of a comment. Run through the subject until we
    // find the closing */ marker
    do {
        lex.cursor++;
        if (lex.cursor == lex.end_pos || (lex.cursor + 1) == lex.end_pos) {
            goto err_no_end_marker;
        }
    } while (*lex.cursor != '*' || *(lex.cursor + 1) != '/');
    goto create_token;

    create_token:
    {
        lex.cursor += 2;
        return true;
    }

    err_no_end_marker:
    {
        std::stringstream estr;
        estr << "Expected closing */ comment marker but found EOS" << std::endl;
        create_syntax_error_marker(ctx, estr, lex.end_pos);
        return false;
    }
}

} // namespace sqltoast
