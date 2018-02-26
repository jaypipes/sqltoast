/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast.h"

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

// <insert statement> ::= INSERT INTO <table name> <insert columns and source>
//
// <insert columns and source> ::=
//     [ <left paren> <insert column list> <right paren> ] <query expression>
//     | DEFAULT VALUES
//
// <insert column list> ::= <column name list>
bool parse_insert(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    lexeme_t table_name;
    symbol_t cur_sym;

    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_INSERT) {
        ctx.lexer.cursor = start;
        return false;
    }
    cur_tok = lex.next();
    goto expect_into;
expect_into:
    // We get here after successfully finding INSERT. We now need to find the
    // INTO symbol.
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_INTO)
        goto err_expect_into;
    cur_tok = lex.next();
    goto expect_identifier;
err_expect_into:
    expect_error(ctx, SYMBOL_INTO);
    return false;
expect_identifier:
    // We get here after successfully finding INSERT followed by INTO. We
    // now need to find the table identifier
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    table_name = cur_tok.lexeme;
    cur_tok = lex.next();
    goto insert_col_list_or_default_values;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
insert_col_list_or_default_values:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_DEFAULT) {
        cur_tok = lex.next();
        goto expect_values;
    }
    goto statement_ending;
expect_values:
    // We get here after successfully finding DEFAULT, which must be followed
    // by the VALUES symbol
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_VALUES)
        goto err_expect_values;
    cur_tok = lex.next();
    goto statement_ending;
err_expect_values:
    expect_error(ctx, SYMBOL_VALUES);
    return false;
statement_ending:
    // We get here after successfully parsing the <table name> element,
    // which must be followed by the <insert columns and source> element
    // behaviour clause>
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SEMICOLON || cur_sym == SYMBOL_EOS) {
        goto push_statement;
    }
    expect_any_error(ctx, {SYMBOL_EOS, SYMBOL_SEMICOLON});
    return false;
push_statement:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<insert_t>(table_name);
    return true;
}

} // namespace sqltoast
