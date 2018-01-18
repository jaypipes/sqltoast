/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <iostream>
#include <cctype>
#include <sstream>

#include "context.h"
#include "error.h"
#include "lexer.h"
#include "statement.h"
#include "symbol.h"
#include "token.h"

namespace sqltoast {

parse_result_t parse(parse_input_t& subject) {
    parse_result_t res;
    res.code = PARSE_INPUT_ERROR;
    parse_context_t ctx(res, subject);

    skip_ws(ctx);
    if (ctx.cursor == ctx.end_pos) {
        res.error.assign("Nothing to parse.");
        return res;
    }

    tokenize(ctx);

    token_t &top_tok = ctx.tokens.front();
    token_type_t& tt = top_tok.type;
    switch (tt) {
        case TOKEN_TYPE_KEYWORD:
            parse_statement(ctx);
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

