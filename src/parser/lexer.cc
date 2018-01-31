/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <vector>

#include "context.h"
#include "error.h"
#include "parser/comment.h"
#include "parser/identifier.h"
#include "parser/lexer.h"
#include "parser/literal.h"
#include "parser/keyword.h"
#include "parser/punctuator.h"
#include "parser/token.h"

namespace sqltoast {

void lexer_t::skip_simple_comments() {
    if (! peek_char('-'))
        return;

    cursor++;
    if (! peek_char('-')) {
        cursor--; // rewind
        return;
    }

    // The comment content is from the cursor until we find a newline or EOS
    do {
        cursor++;
    } while (cursor != end_pos && *cursor != '\n');
}

bool next_token(parse_context_t& ctx) {
    ctx.lexer.skip();
    if (token_comment(ctx))
        return true;
    if (ctx.lexer.error != ERR_NONE)
        return false;
    if (token_punctuator(ctx))
        return true;
    if (token_literal(ctx))
        return true;
    if (token_keyword(ctx))
        return true;
    if (token_identifier(ctx))
        return true;
    return false;
}

void tokenize(parse_context_t& ctx) {
    while (next_token(ctx)) {};
#ifdef SQLTOAST_DEBUG
    std::cout << "tokenize() produced:" << std::endl << "  ";
    for (auto it = ctx.tokens.begin(); it != ctx.tokens.end(); it++) {
        std::cout << *it << " > ";
    }
    std::cout << "EOS" << std::endl;
#endif /* SQLTOAST_DEBUG */
    return;
}

} // namespace sqltoast
