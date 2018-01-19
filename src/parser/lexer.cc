/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <cctype>
#include <sstream>
#include <vector>

#include "context.h"
#include "error.h"
#include "parser/comment.h"
#include "parser/identifier.h"
#include "parser/lexer.h"
#include "parser/keyword.h"
#include "parser/punctuator.h"
#include "parser/token.h"

namespace sqltoast {

bool next_token(parse_context_t& ctx) {
    skip_ws(ctx);
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    if (token_comment(ctx))
        return true;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    if (token_punctuator(ctx))
        return true;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    if (token_keyword(ctx))
        return true;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    if (token_identifier(ctx))
        return true;
    return false;
}

void tokenize(parse_context_t& ctx) {
    while (next_token(ctx)) {};
#ifdef SQLTOAST_DEBUG
    std::cout << "tokenize() produced:" << std::endl << "  ";
    for (auto tok : ctx.tokens) {
        std::cout << tok << " > ";
    }
    std::cout << "EOS" << std::endl;
#endif /* SQLTOAST_DEBUG */
    return;
}

} // namespace sqltoast
