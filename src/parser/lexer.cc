/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <cctype>
#include <sstream>
#include <vector>

#include "comment.h"
#include "context.h"
#include "error.h"
#include "identifier.h"
#include "lexer.h"
#include "keyword.h"
#include "punctuator.h"
#include "symbol.h"
#include "token.h"

namespace sqltoast {

bool accept(parse_context_t& ctx, token_type_t tt) {
    if (ctx.tokens.back().type == tt) {
        return true;
    }
    return false;
}

bool expect(parse_context_t& ctx, token_type_t tt) {
    if (accept(ctx, tt))
        return true;

    token_type_t cur_tt = ctx.tokens.back().type;
    std::stringstream estr;
    estr << "Expected token " << token_type_map::to_string(tt).data() <<
            " but found " << token_type_map::to_string(cur_tt).data() << std::endl;
    create_syntax_error_marker(ctx, estr, parse_position_t(ctx.cursor));
    ctx.result.error.assign(estr.str());
    return false;
}

bool next_token(parse_context_t& ctx) {
    skip_ws(ctx);
    if (token_comment(ctx) ||
            token_punctuator(ctx) ||
            token_keyword(ctx) ||
            token_identifier(ctx)) {
        return true;
    }
    return false;
}

void skip_ws(parse_context_t& ctx) {
    while (std::isspace(*ctx.cursor))
        ctx.cursor++;
    return;
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
