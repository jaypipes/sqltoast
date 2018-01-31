/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#ifndef SQLTOAST_PARSER_SEQUENCE_H
#define SQLTOAST_PARSER_SEQUENCE_H

#include "context.h"
#include "symbol.h"

namespace sqltoast {

// Returns true if the next sequence of symbols found in the tokens stack
// follows an expected pattern. Logs a syntax error on the parse context if the
// expected sequence of symbols was not found.
inline bool follows_sequence(parse_context_t& ctx, const symbol_t expected_sequence[], size_t num_expected) {
    symbol_t exp_sym;
    symbol_t cur_sym;
    token_t* cur_tok;
    for (unsigned int x = 0; x < num_expected; x++) {
        exp_sym = expected_sequence[x];
        cur_tok = next_token(ctx);
        if (cur_tok == NULL)
            goto err_unexpected;
        cur_sym = (*cur_tok).symbol;
        if (cur_sym != exp_sym)
            goto err_unexpected;
    }
    return true;
err_unexpected:
    {
        std::stringstream estr;
        parse_position_t err_pos = ctx.lexer.cursor;
        if (cur_tok == NULL) {
            estr << "Expected " << symbol_map::to_string(exp_sym) << " but found EOS";
        } else {
            cur_sym = (*cur_tok).symbol;
            estr << "Expected " << symbol_map::to_string(exp_sym) << " but found "
                 << symbol_map::to_string(cur_sym);
        }
        estr << std::endl;
        create_syntax_error_marker(ctx, estr, err_pos);
        return false;
    }
}

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_SEQUENCE_H */
