/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

bool parse_drop_view(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    lexeme_t table_name;
    symbol_t cur_sym;
    drop_behaviour_t behaviour = DROP_BEHAVIOUR_CASCADE;

    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_VIEW) {
        // rewind
        ctx.lexer.cursor = start;
        return false;
    }
    cur_tok = lex.next();
    // We get here after successfully finding DROP followed by SCHEMA. We
    // now need to find the table/view identifier
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    table_name = cur_tok.lexeme;
    cur_tok = lex.next();
    goto drop_behaviour_or_statement_ending;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
drop_behaviour_or_statement_ending:
    // We get here after successfully parsing the <table name> element,
    // which must be followed by either a statement ending or a <drop
    // behaviour clause>
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_CASCADE || cur_sym == SYMBOL_RESTRICT) {
        if (cur_sym == SYMBOL_RESTRICT)
            behaviour = DROP_BEHAVIOUR_RESTRICT;
        cur_tok = lex.next();
    }
    goto statement_ending;
statement_ending:
    // We get here after successfully parsing the statement and now expect
    // either the end of parse content or a semicolon to indicate end of
    // statement.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SEMICOLON || cur_sym == SYMBOL_EOS)
        goto push_statement;
    expect_any_error(ctx, {SYMBOL_EOS, SYMBOL_SEMICOLON});
    return false;
push_statement:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<drop_view_statement_t>(
            table_name, behaviour);
    return true;
}

} // namespace sqltoast
