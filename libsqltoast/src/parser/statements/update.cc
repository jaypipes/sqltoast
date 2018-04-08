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

// <update statement: searched> ::=
//     UPDATE <table name> SET <set clause list> [ WHERE <search condition> ]
//
// <set clause list> ::= <set clause> [ { <comma> <set clause> } ... ]
//
// <set clause> ::= <object column> <equals operator> <update source>
//
// <object column> ::= <column name>
//
// <update source> ::= <value expression> | <null specification> | DEFAULT
bool parse_update(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = ctx.lexer.cursor;
    lexeme_t table_name;
    lexeme_t column_name;
    symbol_t cur_sym;
    std::unique_ptr<value_expression_t> value;
    std::vector<set_column_t> set_columns;
    std::unique_ptr<search_condition_t> where_cond;

    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_UPDATE) {
        ctx.lexer.cursor = start;
        return false;
    }
    cur_tok = lex.next();
    goto expect_identifier;
expect_identifier:
    // We get here after successfully finding UPDATE. We now need to find the
    // table identifier
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    table_name = cur_tok.lexeme;
    cur_tok = lex.next();
    goto expect_set;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
expect_set:
    // We get here after successfully finding UPDATE <table name>. We now
    // expect to find the SET symbol followed by the set columns list.
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_SET)
        goto err_expect_set;
    cur_tok = lex.next();
    goto process_set_column;
err_expect_set:
    expect_error(ctx, SYMBOL_SET);
    return false;
process_set_column:
    // Each set column is a column name followed by an equals sign and then the
    // NULL symbol, the DEFAULT symbol or a value expreession.
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    column_name = cur_tok.lexeme;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_EQUAL)
        goto err_expect_equal;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_NULL) {
        cur_tok = lex.next();
        set_columns.emplace_back(set_column_t(SET_COLUMN_TYPE_NULL, column_name));
    } else if (cur_sym == SYMBOL_DEFAULT) {
        cur_tok = lex.next();
        set_columns.emplace_back(set_column_t(SET_COLUMN_TYPE_DEFAULT, column_name));
    } else {
        if (! parse_value_expression(ctx, cur_tok, value))
            goto err_expect_value_null_or_default;
        set_columns.emplace_back(set_column_t(column_name, value));
    }
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_set_column;
    }
    goto optional_where;
err_expect_equal:
    expect_error(ctx, SYMBOL_EQUAL);
    return false;
err_expect_value_null_or_default:
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    {
        std::stringstream estr;
        estr << "Expected to find NULL, DEFAULT or a << value expression >> for "
                "WHERE clause." << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
optional_where:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_WHERE) {
        cur_tok = lex.next();
        if (! parse_search_condition(ctx, cur_tok, where_cond))
            return false;
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
    if (where_cond)
        out = std::make_unique<update_statement_t>(
                table_name, set_columns, where_cond);
    else
        out = std::make_unique<update_statement_t>(table_name, set_columns);
    return true;
}

} // namespace sqltoast
