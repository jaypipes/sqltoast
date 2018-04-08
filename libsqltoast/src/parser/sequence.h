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
// Returns true if the next sequence of symbols found in the supplied lexer
// follows an expected supplied pattern. Uses
// lexer_t::peek_from(parse_position_t) and therefore does not move the
// lexer's underlying cursor state.
inline bool has_sequence(lexer_t& lex, const symbol_t expected_sequence[], size_t num_expected) {
    parse_position_t cur = lex.cursor;
    symbol_t found_sym = lex.current_token.symbol;
    for (unsigned int x = 0; x < num_expected; x++) {
        if (found_sym != expected_sequence[x])
            return false;
        cur = lex.peek_from(cur, &found_sym);
    }
    return true;
}

// Returns true if the next sequence of symbols found in the tokens stack
// follows an expected pattern. Logs a syntax error on the parse context if the
// expected sequence of symbols was not found.
inline bool expect_sequence(parse_context_t& ctx, const symbol_t expected_sequence[], size_t num_expected) {
    symbol_t exp_sym;
    symbol_t cur_sym;
    token_t& cur_tok = ctx.lexer.current_token;
    for (unsigned int x = 0; x < num_expected; x++) {
        exp_sym = expected_sequence[x];
        cur_sym = cur_tok.symbol;
        if (cur_sym != exp_sym)
            goto err_unexpected;
        if (x < (num_expected - 1))
            cur_tok = ctx.lexer.next();
    }
    return true;
err_unexpected:
    {
        std::stringstream estr;
        estr << "Expected " << exp_sym << " but found " << cur_sym;
        estr << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
}

} // namespace sqltoast

#endif /* SQLTOAST_PARSER_SEQUENCE_H */
