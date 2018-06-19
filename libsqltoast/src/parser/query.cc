/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

// <query expression> ::=
//     <non-join query expression>
//     | <joined table>
//
// <query term> ::=
//     <non-join query term>
//     | <joined table>
//
// <query primary> ::=
//     <non-join query primary>
//     | <joined table>
bool parse_query_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<query_expression_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = lex.cursor;
    token_t start_tok = lex.current_token;
    std::unique_ptr<table_reference_t> joined_table;

    if (parse_non_join_query_expression(ctx, cur_tok, out))
        return true;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;

    // Reset cursor to before parsing of joined table attempt.
    lex.cursor = start;
    lex.current_token = cur_tok = start_tok;
    if (! parse_joined_table(ctx, cur_tok, joined_table))
        return false;
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<joined_table_query_expression_t>(joined_table);
    return true;
}

// <non-join query expression> ::=
//     <non-join query term>
//     | <query expression> UNION
//     [ ALL ] [ <corresponding spec> ] <query term>
//     | <query expression> EXCEPT
//     [ ALL ] [ <corresponding spec> ] <query term>
bool parse_non_join_query_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<query_expression_t>& out) {
    std::unique_ptr<non_join_query_term_t> njqt;

    if (! parse_non_join_query_term(ctx, cur_tok, njqt))
        return false;
    // TODO(jaypipes): Handle UNION and EXCEPT
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<non_join_query_expression_t>(njqt);
    return true;
}

// <non-join query term> ::=
//     <non-join query primary>
//     | <query term> INTERSECT
//     [ ALL ] [ <corresponding spec> ] <query primary>
bool parse_non_join_query_term(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<non_join_query_term_t>& out) {
    std::unique_ptr<non_join_query_primary_t> njqp;

    if (! parse_non_join_query_primary(ctx, cur_tok, njqp))
        return false;
    // TODO(jaypipes): Handle INTERSECT
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<non_join_query_term_t>(njqp);
    return true;
}

// <non-join query primary> ::=
//     <simple table>
//     | <left paren> <non-join query expression> <right paren>
//
// <simple table> ::=
//     <query specification>
//     | <table value constructor>
//     | <explicit table>
bool parse_non_join_query_primary(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<non_join_query_primary_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = lex.cursor;
    token_t start_tok = lex.current_token;
    std::unique_ptr<query_specification_t> query_spec;
    std::unique_ptr<table_value_constructor_t> tvc;

    if (parse_query_specification(ctx, cur_tok, query_spec))
        goto push_primary;
    if (ctx.result.code == PARSE_SYNTAX_ERROR)
        return false;
    // Reset cursor to before parsing of joined table attempt.
    lex.cursor = start;
    lex.current_token = cur_tok = start_tok;
    if (! parse_table_value_constructor(ctx, cur_tok, tvc))
        return false;
    // TODO(jaypipes): explicit table
push_primary:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (query_spec)
        out = std::make_unique<query_specification_non_join_query_primary_t>(query_spec);
    else if (tvc)
        out = std::make_unique<table_value_constructor_non_join_query_primary_t>(tvc);
    return true;
}

//  <table value constructor> ::=
//      VALUES <table value constructor list>
//
//  <table value constructor list> ::=
//      <row value constructor>
//      [ { <comma> <row value constructor> }... ]
bool parse_table_value_constructor(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<table_value_constructor_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;

    std::vector<std::unique_ptr<row_value_constructor_t>> val_list;
    std::unique_ptr<row_value_constructor_t> val_list_item;

    if (cur_sym != SYMBOL_VALUES)
        return false;
    cur_tok = lex.next();
    goto process_value_list_item;
process_value_list_item:
    if (! parse_row_value_constructor(ctx, cur_tok, val_list_item))
        goto err_expect_value_item;
    val_list.emplace_back(std::move(val_list_item));
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_value_list_item;
    }
    goto push_tvc;
err_expect_value_item:
    {
        std::stringstream estr;
        estr << "Expected a value item, but got " << cur_tok << "." << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
push_tvc:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<table_value_constructor_t>(val_list);
    return true;
}

// <joined table> ::=
//     <cross join>
//     | <qualified join>
//     | <left paren> <joined table> <right paren>
bool parse_joined_table(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<table_reference_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = lex.cursor;
    token_t start_tok = lex.current_token;
    symbol_t cur_sym = cur_tok.symbol;
    join_type_t join_type = JOIN_TYPE_NONE;
    std::unique_ptr<join_target_t> join_target;
    std::unique_ptr<join_specification_t> join_spec;
    std::unique_ptr<search_condition_t> join_cond;
    std::unique_ptr<table_reference_t> right;
    // Used for the USING clause
    std::vector<lexeme_t> named_columns;

    if (parse_table_reference(ctx, cur_tok, out))
        goto process_join_type;
    // Reset cursor to before parsing of table reference attempt. If the next
    // symbol is a LPAREN, pop it and try parsing a parens-enclosed
    // joined_table construct
    lex.cursor = start;
    lex.current_token = cur_tok = start_tok;
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        return false;
    cur_tok = lex.next();
    goto process_enclosed_joined_table;
process_join_type:
    // We get here after successfully parsing a table reference for the left
    // side of a joined table.  We must now check for symbols that indicate
    // what type of join follows
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_CROSS:
            cur_tok = lex.next();
            join_type = JOIN_TYPE_CROSS;
            goto expect_join;
        case SYMBOL_INNER:
            cur_tok = lex.next();
            cur_sym = cur_tok.symbol;
            if (cur_sym != SYMBOL_JOIN)
                goto err_expect_join;
            cur_tok = lex.next();
            join_type = JOIN_TYPE_INNER;
            goto optional_join_specification;
        case SYMBOL_JOIN:
            cur_tok = lex.next();
            join_type = JOIN_TYPE_INNER;
            goto expect_right_table_reference;
        case SYMBOL_LEFT:
            cur_tok = lex.next();
            join_type = JOIN_TYPE_LEFT;
            goto optional_outer;
        case SYMBOL_RIGHT:
            cur_tok = lex.next();
            join_type = JOIN_TYPE_RIGHT;
            goto optional_outer;
        case SYMBOL_FULL:
            cur_tok = lex.next();
            join_type = JOIN_TYPE_FULL;
            goto optional_outer;
        case SYMBOL_NATURAL:
            cur_tok = lex.next();
            join_type = JOIN_TYPE_NATURAL;
            goto expect_join;
        case SYMBOL_UNION:
            // NOTE(jaypipes): UNION JOIN was removed in ANSI-SQL 2003
            cur_tok = lex.next();
            join_type = JOIN_TYPE_UNION;
            goto expect_join;
        default:
            return false;
    }
expect_join:
    // We get here after successfully parsing a normal or derived table
    // followed by the UNION, CROSS or NATURAL symbol. We now expect a JOIN
    // symbol followed by another <table_reference>
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_JOIN)
        goto err_expect_join;
    cur_tok = lex.next();
    goto expect_right_table_reference;
expect_right_table_reference:
    if (! parse_table_reference(ctx, cur_tok, right))
        goto err_expect_table_reference;
    goto optional_join_specification;
err_expect_join:
    expect_error(ctx, SYMBOL_JOIN);
    return false;
err_expect_table_reference:
    {
        std::stringstream estr;
        estr << "Expected <table reference> but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
optional_join_specification:
    // We get here after successfully parsing an INNER or OUTER symbol followed
    // by a JOIN symbol and a <table reference>. We now must check for the
    // optional <join specification> clause
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_ON:
            cur_tok = lex.next();
            goto process_join_condition;
        case SYMBOL_USING:
            cur_tok = lex.next();
            goto process_named_columns;
        default:
            goto push_joined_table;
    }
process_join_condition:
    // We get here after parsing the joined table references and finding an ON
    // symbol. We now expect to find a search condition
    if (! parse_search_condition(ctx, cur_tok, join_cond))
        goto err_expect_join_condition;
    goto push_joined_table;
err_expect_join_condition:
    {
        std::stringstream estr;
        estr << "Expected <join condition> but found "
             << cur_tok << std::endl;
        create_syntax_error_marker(ctx, estr);
        return false;
    }
process_named_columns:
    // We get here after parsing a USING symbol, which must be followed by a
    // parens-enclosed list of column identifiers
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_LPAREN)
        goto err_expect_lparen;
    cur_tok = lex.next();
    goto process_named_column;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
process_named_column:
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    named_columns.emplace_back(cur_tok.lexeme);
    cur_tok = lex.next();
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_COMMA) {
        cur_tok = lex.next();
        goto process_named_column;
    }
    else if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto push_joined_table;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
optional_outer:
    // We get here after successfully parsing the FULL, LEFT or RIGHT symbols.
    // These symbols may be followed by an optional OUTER symbol and then the
    // required JOIN symbol.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_OUTER)
        cur_tok = lex.next();
    goto expect_join;
process_enclosed_joined_table:
    return false;
push_joined_table:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (! named_columns.empty())
        join_spec = std::make_unique<join_specification_t>(named_columns);
    else if (join_cond)
        join_spec = std::make_unique<join_specification_t>(join_cond);
    join_target = std::make_unique<join_target_t>(join_type, right, join_spec);
    out->join(join_target);
    return true;
}

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
bool parse_query_specification(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<query_specification_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::unique_ptr<value_expression_t> selected_col;
    std::vector<derived_column_t> selected_columns;
    std::unique_ptr<table_expression_t> table_expression;
    bool distinct = false;

    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_SELECT)
        return false;
    cur_tok = lex.next();
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
    goto expect_table_expression;
err_expect_comma_or_from:
    expect_any_error(ctx, {SYMBOL_COMMA, SYMBOL_FROM});
    return false;
expect_table_expression:
    if (! parse_table_expression(ctx, cur_tok, table_expression))
        return false;
    goto push_query_specification;
push_query_specification:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<query_specification_t>(
            distinct, selected_columns, table_expression);
    return true;
}

// <table expression> ::=
//     <from clause>
//     [ <where clause> ]
//     [ <group by clause> ]
//     [ <having clause> ]
bool parse_table_expression(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<table_expression_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    std::vector<std::unique_ptr<table_reference_t>> referenced_tables;
    std::unique_ptr<table_reference_t> table_ref;
    std::vector<grouping_column_reference_t> group_by_columns;
    std::unique_ptr<search_condition_t> where_condition;
    std::unique_ptr<search_condition_t> having_condition;

    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_FROM)
        return false;
    cur_tok = lex.next();
    goto expect_table_reference;
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
            goto push_table_expression;;
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
            goto push_table_expression;
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
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
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
    goto push_table_expression;
having_or_statement_ending:
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_HAVING) {
        cur_tok = lex.next();
        goto process_having;
    }
    goto push_table_expression;
push_table_expression:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<table_expression_t>(
            referenced_tables, where_condition, group_by_columns,
            having_condition);
    return true;
}

} // namespace sqltoast
