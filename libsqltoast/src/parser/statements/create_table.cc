/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include <sstream>

#include "sqltoast/sqltoast.h"

#include "parser/parse.h"
#include "parser/error.h"

namespace sqltoast {

//
// The CREATE TABLE statement follows this EBNF form for the following SQL
// dialects:
//
// * SQL_DIALECT_ANSI_1992
//
// <table definition> ::=
//     CREATE [{GLOBAL|LOCAL} TEMPORARY] TABLE <table name>
//     <table element list>
//     [ON COMMIT {DELETE|PRESERVE} ROWS]
//
// TODO: Maybe support the ON COMMIT modifier (it's very rarely used)
bool parse_create_table(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    lexeme_t table_name;
    symbol_t cur_sym;
    table_type_t table_type = TABLE_TYPE_NORMAL;
    std::unique_ptr<column_definition_t> column_def;
    std::vector<std::unique_ptr<column_definition_t>> column_defs;
    std::unique_ptr<constraint_t> constraint;
    std::vector<std::unique_ptr<constraint_t>> constraints;

    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_ERROR:
            return false;
        case SYMBOL_TABLE:
            cur_tok = lex.next();
            goto expect_table_name;
        case SYMBOL_GLOBAL:
        case SYMBOL_LOCAL:
        case SYMBOL_TEMPORARY:
            goto table_kw_or_table_type;
        default:
            // rewind
            ctx.lexer.cursor = start;
            return false;
    }
table_kw_or_table_type:
    // We get here after successfully finding the CREATE symbol. We can
    // either match the table keyword or the table type clause
    if (cur_sym == SYMBOL_GLOBAL) {
        table_type = TABLE_TYPE_TEMPORARY_GLOBAL;
        cur_tok = lex.next();
        goto expect_temporary;
    } else if (cur_sym == SYMBOL_LOCAL) {
        table_type = TABLE_TYPE_TEMPORARY_LOCAL;
        cur_tok = lex.next();
        goto expect_temporary;
    } else if (cur_sym == SYMBOL_TEMPORARY) {
        table_type = TABLE_TYPE_TEMPORARY_GLOBAL;
        cur_tok = lex.next();
        goto expect_table;
    }
expect_temporary:
    // We get here if we successfully matched CREATE followed by either the
    // GLOBAL or LOCAL symbol. If this is the case, we expect to find the
    // TEMPORARY keyword followed by the TABLE keyword.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_TEMPORARY) {
        cur_tok = lex.next();
        goto expect_table;
    }
    goto err_expect_temporary;
err_expect_temporary:
    expect_error(ctx, SYMBOL_TEMPORARY);
    return false;
expect_table:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_TABLE) {
        cur_tok = lex.next();
        goto expect_table_name;
    }
    goto err_expect_table;
err_expect_table:
    expect_error(ctx, SYMBOL_TABLE);
    return false;
expect_table_name:
    // We get here after successfully finding CREATE followed by the TABLE
    // symbol (after optionally processing the table type modifier). We now
    // need to find an identifier
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_IDENTIFIER) {
        table_name = cur_tok.lexeme;
        cur_tok = lex.next();
        goto expect_table_list_open;
    }
    goto err_expect_identifier;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
expect_table_list_open:
    // We get here after successfully finding the CREATE ... TABLE <table name>
    // part of the statement. We now expect to find the <table element
    // list> clause
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_LPAREN) {
        cur_tok = lex.next();
        goto expect_table_list_element;
    }
    goto err_expect_lparen;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
expect_table_list_element:
    // We get here after finding the LPAREN opening the <table element
    // list> clause. Now we expect to find one or more column or constraint
    // definitions
    if (parse_column_definition(ctx, cur_tok, column_def)) {
        column_defs.emplace_back(std::move(column_def));
        goto expect_table_list_close;
    }
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    if (! parse_constraint(ctx, cur_tok, constraint))
        goto err_expect_column_def_or_constraint;
    goto expect_table_list_close;
err_expect_column_def_or_constraint:
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    else {
        std::stringstream estr;
        estr << "Expected either a column definition or a "
                "constraint but found " << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
expect_table_list_close:
    // We get here after successfully parsing the <table element list>
    // column/constraint definitions and are now expecting the closing
    // RPAREN to indicate the end of the <table element list>
    cur_tok = lex.current_token;
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_RPAREN:
            cur_tok = lex.next();
            goto statement_ending;
        case SYMBOL_COMMA:
            cur_tok = lex.next();
            goto expect_table_list_element;
        default:
            goto err_expect_rparen_or_comma;
    }
err_expect_rparen_or_comma:
    expect_any_error(ctx, {SYMBOL_COMMA, SYMBOL_RPAREN});
    return false;
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
    out = std::make_unique<create_table_statement_t>(
            table_type, table_name, column_defs, constraints);
    return true;
}

} // namespace sqltoast
