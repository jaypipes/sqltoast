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
#include "parser/statement.h"
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
    token_t* cur_tok;

    if (lex.cursor == lex.end_pos) {
        res.code = PARSE_INPUT_ERROR;
        res.error.assign("Nothing to parse.");
        return res;
    }

    while (res.code == PARSE_OK) {
        cur_tok = lex.next_token();
        if (cur_tok == NULL)
            return res;
        if (cur_tok->is_keyword()) {
            parse_statement(ctx);
            continue;
        }
        if (cur_tok->is_punctuator()) {
            std::stringstream estr;
            estr << "Parse subject must either begin with a keyword or a "
                    "comment, but found punctuation." << std::endl;
            create_syntax_error_marker(ctx, estr, parse_position_t(lex.cursor));
            continue;
        }
        if (cur_tok->is_literal()) {
            std::stringstream estr;
            estr << "Parse subject must either begin with a keyword or a "
                    "comment, but found literal." << std::endl;
            create_syntax_error_marker(ctx, estr, parse_position_t(lex.cursor));
            continue;
        }
        if (cur_tok->is_identifier()) {
            std::stringstream estr;
            estr << "Parse subject must either begin with a keyword or a "
                    "comment, but found identifier." << std::endl;
            create_syntax_error_marker(ctx, estr, parse_position_t(lex.cursor));
            continue;
        }
    }

    return res;
}

} // namespace sqltoast

