/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <cctype>

#include "comment.h"
#include "context.h"
#include "identifier.h"
#include "keyword.h"
#include "punctuator.h"
#include "statements/create_database.h"
#include "token.h"

namespace sqltoast {

bool accept(parse_context_t& ctx, token_type_t tt) {
    if (ctx.tokens.top().type == tt) {
        return true;
    }
    return false;
}

bool expect(parse_context_t& ctx, token_type_t tt) {
    if (accept(ctx, tt))
        return true;

    char estr[200];
    sprintf(estr, "Expected token %s but found %s",
            token_type_map::to_string(tt).data(),
            token_type_map::to_string(ctx.tokens.top().type).data());
    ctx.result.error.assign(estr);
    return false;
}

void next_symbol(parse_context_t& ctx) {
    skip_ws(ctx);
    if (parse_comment(ctx))
        return;
    if (parse_punctuator(ctx))
        return;
    if (parse_keyword(ctx))
        return;
    if (parse_identifier(ctx))
        return;
    return;
}

void skip_ws(parse_context_t& ctx) {
    while (std::isspace(*ctx.cursor))
        ctx.cursor++;
    return;
}

parse_result_t parse(parse_input_t& subject) {
    parse_result_t res;
    res.code = PARSE_SYNTAX_ERROR;
    parse_context_t ctx(res, subject);

    next_symbol(ctx);

    token_type_t tt = ctx.tokens.top().type;
    switch (tt) {
        case TOKEN_TYPE_KEYWORD:
            next_symbol(ctx);
            if (accept(ctx, TOKEN_TYPE_KEYWORD)) {
                if (parse_create_database(ctx)) {
                    res.code = PARSE_SUCCESS;
                }
            }
        case TOKEN_TYPE_COMMENT:
            res.error.assign("got a comment!");
            res.code = PARSE_SYNTAX_ERROR;
            break;
        default:
            break;
    }

    return res;
}

} // namespace sqltoast
