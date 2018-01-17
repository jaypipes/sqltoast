/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <cctype>
#include <sstream>

#include "comment.h"
#include "context.h"
#include "error.h"
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

    token_type_t cur_tt = ctx.tokens.top().type;
    std::stringstream estr;
    estr << "Expected token " << token_type_map::to_string(tt).data() <<
            " but found " << token_type_map::to_string(cur_tt).data() << std::endl;
    create_syntax_error_marker(ctx, estr, parse_position_t(ctx.cursor));
    ctx.result.error.assign(estr.str());
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
    res.code = PARSE_INPUT_ERROR;
    parse_context_t ctx(res, subject);

    skip_ws(ctx);
    if (ctx.cursor == ctx.end_pos) {
        res.error.assign("Nothing to parse.");
        return res;
    }

    next_symbol(ctx);

    token_type_t tt = ctx.tokens.top().type;
    switch (tt) {
        case TOKEN_TYPE_KEYWORD:
            next_symbol(ctx);
            if (accept(ctx, TOKEN_TYPE_KEYWORD)) {
                if (parse_create_database(ctx)) {
                    res.code = PARSE_SUCCESS;
                    break;
                }
            }
            break;
        case TOKEN_TYPE_COMMENT:
            res.error.assign("got a comment!");
            res.code = PARSE_SYNTAX_ERROR;
            break;
        case TOKEN_TYPE_PUNCTUATOR:
        {
            std::stringstream estr;
            estr << "Parse subject must either begin with a keyword or a "
                    "comment, but found punctuation." << std::endl;
            create_syntax_error_marker(ctx, estr, parse_position_t(ctx.cursor));
            res.code = PARSE_SYNTAX_ERROR;
            res.error.assign(estr.str());
        }
        break;
        case TOKEN_TYPE_IDENTIFIER:
        {
            std::stringstream estr;
            estr << "Parse subject must either begin with a keyword or a "
                    "comment, but found identifier." << std::endl;
            create_syntax_error_marker(ctx, estr, parse_position_t(ctx.cursor));
            res.code = PARSE_SYNTAX_ERROR;
            res.error.assign(estr.str());
        }
        break;
        default:
            break;
    }

    return res;
}

} // namespace sqltoast
