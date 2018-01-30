/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "parser/error.h"
#include "parser/comment.h"
#include "parser/peek.h"
#include "parser/symbol.h"
#include "parser/token.h"

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
    if (! peek_char(ctx, '-'))
        return false;

    ctx.cursor++;
    if (! peek_char(ctx, '-')) {
        ctx.cursor--; // rewind
        return false;
    }

    parse_position_t start = ctx.cursor - 2;

    // The comment content is from the cursor until we find a newline of EOS
    do {
        ctx.cursor++;
    } while (ctx.cursor != ctx.end_pos && *ctx.cursor != '\n');
    return true;
}

bool token_bracketed_comment(parse_context_t& ctx) {
    if (! peek_char(ctx, '/'))
        return false;

    ctx.cursor++;
    if (! peek_char(ctx, '*')) {
        ctx.cursor--; // rewind
        return false;
    }

    parse_position_t start = ctx.cursor - 2;

    // OK, we found the start of a comment. Run through the subject until we
    // find the closing */ marker
    do {
        ctx.cursor++;
        if (ctx.cursor == ctx.end_pos || (ctx.cursor + 1) == ctx.end_pos) {
            goto err_no_end_marker;
        }
    } while (*ctx.cursor != '*' || *(ctx.cursor + 1) != '/');
    goto create_token;

    create_token:
    {
        ctx.cursor += 2;
        return true;
    }

    err_no_end_marker:
    {
        std::stringstream estr;
        estr << "Expected closing */ comment marker but found EOS" << std::endl;
        create_syntax_error_marker(ctx, estr, ctx.end_pos);
        return false;
    }
}

} // namespace sqltoast
