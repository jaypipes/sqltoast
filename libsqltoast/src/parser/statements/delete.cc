/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "sqltoast/sqltoast.h"

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

// <delete statement: positioned> ::=
//     DELETE FROM <table name> WHERE CURRENT OF <cursor name>
//
// <delete statement: searched> ::=
//     DELETE FROM <table name> [ WHERE <search condition> ]
bool parse_delete(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    lexeme_t table_name;
    symbol_t cur_sym;
    std::unique_ptr<search_condition_t> where_cond;

    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_DELETE) {
        ctx.lexer.cursor = start;
        return false;
    }
    cur_tok = lex.next();
    goto expect_from;
expect_from:
    // We get here after successfully finding DELETE. We now need to find the
    // FROM symbol.
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_FROM)
        goto err_expect_from;
    cur_tok = lex.next();
    goto expect_identifier;
err_expect_from:
    expect_error(ctx, SYMBOL_FROM);
    return false;
expect_identifier:
    // We get here after successfully finding DELETE followed by FROM. We
    // now need to find the table identifier
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    table_name = cur_tok.lexeme;
    cur_tok = lex.next();
    goto opt_search_condition;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
opt_search_condition:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_WHERE)
        goto statement_ending;
    cur_tok = lex.next();
    if (! parse_search_condition(ctx, cur_tok, where_cond))
        return false;
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
    out = std::make_unique<delete_statement_t>(table_name, where_cond);
    return true;
}

} // namespace sqltoast
