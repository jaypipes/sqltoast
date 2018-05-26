/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

// <view definition> ::=
//     CREATE VIEW <table name> [ <left paren> <view column list> <right paren> ]
//     AS <query expression> [ WITH [ <levels clause> ] CHECK OPTION ]
//
// <view column list> ::= <column name list>
//
// <levels clause> ::= CASCADED | LOCAL
bool parse_create_view(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    symbol_t cur_sym = cur_tok.symbol;
    lexeme_t table_name;
    check_option_t check_option = CHECK_OPTION_NONE;
    std::vector<lexeme_t> columns;
    std::unique_ptr<query_expression_t> query;

    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_VIEW) {
        // rewind
        ctx.lexer.cursor = start;
        return false;
    }
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    table_name = cur_tok.lexeme;
    cur_tok = lex.next();
    goto optional_column_list;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
optional_column_list:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_LPAREN) {
        cur_tok = lex.next();
        goto process_column_list_element;
    }
    goto expect_as;
process_column_list_element:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    columns.emplace_back(lexeme_t(cur_tok.lexeme));
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_column_list_element;
    }
    goto expect_rparen;
expect_rparen:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto expect_as;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
expect_as:
    // We get here after successfully parsing the CREATE VIEW <table name> with
    // the optional column list. We now expect the AS symbol followed by a
    // query expression
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_AS)
        goto err_expect_as;
    cur_tok = lex.next();
    goto expect_query_expression;
err_expect_as:
    expect_error(ctx, SYMBOL_AS);
    return false;
expect_query_expression:
    if (! parse_query_expression(ctx, cur_tok, query))
        goto err_expect_query_expression;
    goto optional_check_option;
optional_check_option:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_WITH) {
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym == SYMBOL_CASCADED) {
            check_option = CHECK_OPTION_CASCADED;
            cur_tok = lex.next();
        } else if (cur_sym == SYMBOL_LOCAL) {
            check_option = CHECK_OPTION_LOCAL;
            cur_tok = lex.next();
        } else
            goto err_expect_cascaded_or_local;
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_CHECK)
            goto err_expect_check;
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_OPTION)
            goto err_expect_option;
        cur_tok = lex.next();
    }
    goto statement_ending;
err_expect_cascaded_or_local:
    expect_any_error(ctx, {SYMBOL_CASCADED, SYMBOL_LOCAL});
    return false;
err_expect_check:
    expect_error(ctx, SYMBOL_CHECK);
    return false;
err_expect_option:
    expect_error(ctx, SYMBOL_OPTION);
    return false;
err_expect_query_expression:
    {
        std::stringstream estr;
        estr << "Expected to find <query expression> but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
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
    out = std::make_unique<create_view_statement_t>(
            table_name, check_option, columns, query);
    return true;
}

} // namespace sqltoast
