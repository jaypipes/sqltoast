/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <cctype>
#include <sstream>

#include "parser/context.h"
#include "parser/error.h"
#include "parser/lexer.h"
#include "parser/parse.h"
#include "parser/token.h"

namespace sqltoast {

parse_result_t parse(parse_input_t& subject) {
    parse_options_t opts = {
        SQL_DIALECT_ANSI_1992,
        false
    };

    return parse(subject, opts);
}

parse_result_t parse(parse_input_t& subject, parse_options_t& opts) {
    parse_result_t res;
    res.code = PARSE_OK;
    parse_context_t ctx(res, opts, subject);
    lexer_t& lex = ctx.lexer;
    token_t& cur_tok = lex.current_token;

    if (lex.cursor == lex.end) {
        res.code = PARSE_INPUT_ERROR;
        res.error.assign("Nothing to parse.");
        return res;
    }
    cur_tok = lex.next();

    while (res.code == PARSE_OK) {
        if (cur_tok.symbol == SYMBOL_EOS)
            break;
        if (cur_tok.symbol == SYMBOL_SEMICOLON) {
            cur_tok = lex.next();
            continue;
        }
        if (cur_tok.is_keyword()) {
            parse_statement(ctx);
            continue;
        }
        {
            std::stringstream estr;
            estr << "SQL statements begin with a keyword and end with a "
                    "semicolon, but found " << cur_tok << "." << std::endl;
            create_syntax_error_marker(ctx, estr);
            continue;
        }
    }

    return res;
}

} // namespace sqltoast
