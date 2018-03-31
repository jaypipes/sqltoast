/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "sqltoast.h"

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

// The SELECT statement follows this EBNF form for the ANSI-92 SQL dialect:
//
// <query specification> ::=
//     SELECT [ <set quantifier> ] <select list> <table expression>
//
// <set quantifier> ::= DISTINCT | ALL
//
// <select list> ::=
//     <asterisk>
//     | <select sublist> [ { <comma> <select sublist> }... ]
//
// <select sublist> ::= <derived column> | <qualifier> <period> <asterisk>
//
// <derived column> ::= <value expression> [ <as clause> ]
//
// <as clause> ::= [ AS ] <column name>
//
// <table expression> ::=
//     <from clause>
//     [ <where clause> ]
//     [ <group by clause> ]
//     [ <having clause> ]
bool parse_select(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<statement_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_t> selected_col;
    std::vector<derived_column_t> selected_columns;
    std::vector<std::unique_ptr<table_reference_t>> referenced_tables;
    std::unique_ptr<table_reference_t> table_ref;
    std::vector<grouping_column_reference_t> group_by_columns;
    std::unique_ptr<search_condition_t> where_condition;
    std::unique_ptr<search_condition_t> having_condition;
    bool distinct = false;

    // BEGIN STATE MACHINE

    // We get here after successfully finding the SELECT symbol (the current
    // symbol in the lexer is the SELECT symbol), which can optionally be
    // followed by the DISTINCT or ALL symbol and then the <select list> clause

    cur_tok = lex.next(); // Consume the SELECT symbol...
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_DISTINCT) {
        cur_tok = lex.next();
        distinct = true;
    } else if (cur_sym == SYMBOL_ALL) {
        cur_tok = lex.next();
        distinct = false;
    }
expect_derived_column:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_ASTERISK) {
        selected_columns.emplace_back(derived_column_t());
        cur_tok = lex.next();
        goto comma_or_from;
    }
    // If the <select list> element isn't an asterisk it needs to be a value
    // expression...
    if (! parse_value_expression(ctx, cur_tok, selected_col))
        goto err_expect_derived_column;
    selected_columns.emplace_back(derived_column_t(selected_col));
    goto optional_column_alias;
err_expect_derived_column:
    expect_any_error(ctx, {SYMBOL_ASTERISK, SYMBOL_IDENTIFIER});
    return false;
optional_column_alias:
    // We get here after consuming a column identifier or value expression. An
    // alias can be provided for this column either by specifying an identifier
    // immediately after the value expression or the keyword AS followed by the
    // alias.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_AS) {
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_IDENTIFIER)
            goto err_expect_identifier;
    }
    if (cur_sym == SYMBOL_IDENTIFIER) {
        derived_column_t& dc = selected_columns.back();
        dc.alias = cur_tok.lexeme;
        cur_tok = lex.next();
    }
    goto comma_or_from;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
comma_or_from:
    // We get here after consuming a derived column and now we expect to find
    // either a comma, indicating another derived column should be next, or the
    // FROM symbol, indicating the end of the <select list> clause.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto expect_derived_column;
    }
    if (cur_sym != SYMBOL_FROM)
        goto err_expect_comma_or_from;
    cur_tok = lex.next();
    goto expect_table_reference;
err_expect_comma_or_from:
    expect_any_error(ctx, {SYMBOL_COMMA, SYMBOL_FROM});
    return false;
expect_table_reference:
    if (! parse_table_reference(ctx, cur_tok, table_ref))
        return false;
    referenced_tables.emplace_back(std::move(table_ref));
    goto comma_or_where_group_having;
comma_or_where_group_having:
    // We get here after consuming a table reference and now we expect to find
    // either a comma, indicating another table reference should be next, or
    // one of the GROUP BY, WHERE or HAVING symbols, indicating those clauses
    // are to follow
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_COMMA:
            cur_tok = lex.next();
            goto expect_table_reference;
        case SYMBOL_WHERE:
            cur_tok = lex.next();
            goto process_where;
        case SYMBOL_GROUP:
            cur_tok = lex.next();
            goto expect_by;
        case SYMBOL_HAVING:
            cur_tok = lex.next();
            goto process_having;
        default:
            // TODO
            goto statement_ending;
    }
process_where:
    cur_sym = cur_tok.symbol;
    if (! parse_search_condition(ctx, cur_tok, where_condition))
        return false;
    goto group_by_having_or_statement_ending;
group_by_having_or_statement_ending:
    // We get here after consuming the FROM and optional WHERE clauses. We can
    // now get either the end of statement, the GROUP BY or the HAVING clause
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_GROUP:
            cur_tok = lex.next();
            goto expect_by;
        case SYMBOL_HAVING:
            cur_tok = lex.next();
            goto process_having;
        default:
            goto statement_ending;
    }
expect_by:
    // We get here after finding the GROUP symbol, which must be followed by
    // the BY symbol and then one or more grouping column references
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_BY)
        goto err_expect_by;
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    goto process_group_by_column;
err_expect_by:
    expect_error(ctx, SYMBOL_BY);
    return false;
process_group_by_column:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    group_by_columns.emplace_back(grouping_column_reference_t(cur_tok.lexeme));
    cur_tok = lex.next();
    goto optional_collation;
optional_collation:
    // We get here after consuming an identifier for a grouping column
    // reference and now we can get an optional collation for that column
    // reference
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COLLATE) {
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_IDENTIFIER)
            goto err_expect_identifier;
    }
    if (cur_sym == SYMBOL_IDENTIFIER) {
        grouping_column_reference_t& gcr = group_by_columns.back();
        gcr.collation = cur_tok.lexeme;
        cur_tok = lex.next();
    }
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_group_by_column;
    }
    goto having_or_statement_ending;
process_having:
    cur_sym = cur_tok.symbol;
    if (! parse_search_condition(ctx, cur_tok, having_condition))
        return false;
    goto statement_ending;
having_or_statement_ending:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_HAVING) {
        cur_tok = lex.next();
        goto process_having;
    }
    goto statement_ending;
statement_ending:
    // We get here if we have already successfully processed the SELECT
    // statement and are expecting EOS or SEMICOLON as the next non-comment
    // token. Alternately, we can find an RPAREN, which would indicate we may
    // be at the end of a valid subquery, which is also fine. We rely on the
    // caller to check for the validity of an RPAREN in the current lexical
    // context.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_SEMICOLON ||
            cur_sym == SYMBOL_EOS ||
            cur_sym == SYMBOL_RPAREN)
        goto push_statement;
    expect_any_error(ctx, {SYMBOL_EOS, SYMBOL_SEMICOLON, SYMBOL_RPAREN});
    return false;
push_statement:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<select_statement_t>(
            distinct, selected_columns, referenced_tables, where_condition,
            group_by_columns, having_condition);
    return true;
}

} // namespace sqltoast
