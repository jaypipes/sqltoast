/*
 * Use and distribution licensed under the Apache license version 2.
 *
 * See the COPYING file in the root project directory for full text.
 */

#include "parser/error.h"
#include "parser/parse.h"

namespace sqltoast {

// <table reference> ::=
//     <table name> [ <correlation specification> ]
//     | <derived table> <correlation specification>
//     | <joined table>
//
// <correlation specification> ::=
//     [ AS ] <correlation name>
//     [ <left paren> <derived column list> <right paren> ]
//
// <derived column list> ::= <column name list>
//
// <joined table> ::=
//     <cross join>
//     | <qualified join>
//     | <left paren> <joined table> <right paren>
//
// <cross join>    ::=
//     <table reference> CROSS JOIN <table reference>
//
// <qualified join>    ::=
//     <table reference> [ NATURAL ] [ <join type> ]
//     JOIN <table reference> [ <join specification> ]
//
// <join type> ::=
//     INNER
//     | <outer join type> [ OUTER ]
//     | UNION
//
// <outer join type> ::= LEFT | RIGHT | FULL
//
// <join specification> ::= <join condition> | <named columns join>
//
// <join condition>::= ON <search condition>
//
// <named columns join> ::=
//     USING <left paren> <join column list> <right paren>
//
// <join column list> ::= <column name list>
bool parse_table_reference(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<table_reference_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    lexeme_t table_name;
    lexeme_t alias;
    join_type_t join_type = JOIN_TYPE_NONE;
    std::unique_ptr<join_target_t> join_target;
    std::unique_ptr<join_specification_t> join_spec;
    std::unique_ptr<search_condition_t> join_cond;
    std::unique_ptr<table_reference_t> right;
    // Used for the USING clause
    std::vector<lexeme_t> named_columns;
    if (cur_sym == SYMBOL_IDENTIFIER) {
        table_name = cur_tok.lexeme;
        cur_tok = lex.next();
        goto optional_alias;
    }
    if (! parse_derived_table(ctx, cur_tok, out))
        return false;
    goto check_join;
optional_alias:
    // We get here after consuming an identifier, derived table specifier or
    // joined table specifier. An alias can be provided for this table
    // reference either by specifying an identifier immediately after the value
    // expression or the keyword AS followed by the alias.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_AS) {
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
        if (cur_sym != SYMBOL_IDENTIFIER)
            goto err_expect_identifier;
    }
    if (cur_sym == SYMBOL_IDENTIFIER) {
        alias = cur_tok.lexeme;
        cur_tok = lex.next();
    }
    goto ensure_normal_table;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
check_join:
    // We get here after successfully parsing a normal table or derived table.
    // We must now check for symbols that indicate a joined table specification
    // follows
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_CROSS:
            cur_tok = lex.next();
            join_type = JOIN_TYPE_CROSS;
            goto process_union_cross_or_natural_join;
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
            goto optional_join_specification;
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
            goto process_union_cross_or_natural_join;
        case SYMBOL_UNION:
            // NOTE(jaypipes): UNION JOIN was removed in ANSI-SQL 2003
            cur_tok = lex.next();
            join_type = JOIN_TYPE_UNION;
            goto process_union_cross_or_natural_join;
        default:
            return true;
    }
process_union_cross_or_natural_join:
    // We get here after successfully parsing a normal or derived table
    // followed by the UNION, CROSS or NATURAL symbol. We now expect a JOIN
    // symbol followed by another <table_reference>
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_JOIN)
        goto err_expect_join;
    cur_tok = lex.next();
    if (! parse_table_reference(ctx, cur_tok, right))
        goto err_expect_table_reference;
    goto push_join;
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
    // by a JOIN symbol. We now must check for the optional <join
    // specification> clause
    if (! parse_table_reference(ctx, cur_tok, right))
        goto err_expect_table_reference;
    cur_sym = cur_tok.symbol;
    switch (cur_sym) {
        case SYMBOL_ON:
            cur_tok = lex.next();
            goto process_join_condition;
        case SYMBOL_USING:
            cur_tok = lex.next();
            goto process_named_columns;
        default:
            goto push_join;
    }
process_join_condition:
    // We get here after parsing the joined table references and finding an ON
    // symbol. We now expect to find a search condition
    if (! parse_search_condition(ctx, cur_tok, join_cond))
        goto err_expect_join_condition;
    goto push_join;
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
    goto push_join;
err_expect_lparen:
    expect_error(ctx, SYMBOL_LPAREN);
    return false;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
optional_outer:
    // We get here after successfully parsing the FULL, LEFT or RIGHT symbols.
    // These symbols may be followed by an optional OUTER symbol and then the
    // required JOIN symbol.
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_OUTER) {
        cur_tok = lex.next();
        cur_sym = cur_tok.symbol;
    }
    if (cur_sym != SYMBOL_JOIN)
        goto err_expect_join;
    cur_tok = lex.next();
    goto optional_join_specification;
ensure_normal_table:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<table_t>(table_name, alias);
    goto check_join;
push_join:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (! right) {
        // out parameter has already been populated with either a normal or
        // derived table, and we found no JOIN symbol afterwards, so just
        // return
        return true;
    }
    if (! named_columns.empty())
        join_spec = std::make_unique<join_specification_t>(named_columns);
    else if (join_cond)
        join_spec = std::make_unique<join_specification_t>(join_cond);
    join_target = std::make_unique<join_target_t>(join_type, right, join_spec);
    out->join(join_target);
    return true;
}

// <derived table> ::= <table subquery>
//
// <table subquery> ::= <subquery>
bool parse_derived_table(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<table_reference_t>& out) {
    lexer_t& lex = ctx.lexer;
    symbol_t cur_sym = cur_tok.symbol;
    lexeme_t alias;
    std::unique_ptr<query_expression_t> query;

    if (cur_sym != SYMBOL_LPAREN)
        return false;

    cur_tok = lex.next();
    if (! parse_query_expression(ctx, cur_tok, query))
        goto err_expect_query_expression;

    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;

    cur_tok = lex.next();
    // A derived table MUST have a <correlation specification> which means it
    // must be given a name preceded by an optional AS symbol
    cur_sym = cur_tok.symbol;
    if (cur_sym == SYMBOL_AS) {
        cur_tok = lex.next();
    }
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_IDENTIFIER)
        goto err_expect_identifier;
    alias = cur_tok.lexeme;
    cur_tok = lex.next();
    goto push_derived_table;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
err_expect_query_expression:
{
    std::stringstream estr;
    estr << "Expected <query expression> but found "
         << cur_tok << std::endl;
    create_syntax_error_marker(ctx, estr);
    return false;
}
push_derived_table:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<derived_table_t>(alias, query);
    return true;
}

} // namespace sqltoast
