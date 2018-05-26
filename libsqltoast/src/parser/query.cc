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
    std::unique_ptr<joined_table_t> joined_table;

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
    std::unique_ptr<query_specification_t> query_spec;

    if (! parse_query_specification(ctx, cur_tok, query_spec))
        return false;
    // TODO(jaypipes): Handle table value constructor and explicit table
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<query_specification_non_join_query_primary_t>(
            query_spec);
    return true;
}

// <joined table> ::=
//     <cross join>
//     | <qualified join>
//     | <left paren> <joined table> <right paren>
bool parse_joined_table(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<joined_table_t>& out) {
    lexer_t& lex = ctx.lexer;
    parse_position_t start = lex.cursor;
    token_t start_tok = lex.current_token;
    symbol_t cur_sym = cur_tok.symbol;
    join_type_t join_type = JOIN_TYPE_UNKNOWN;
    std::unique_ptr<search_condition_t> join_cond;
    std::unique_ptr<table_reference_t> left;
    std::unique_ptr<table_reference_t> right;
    // Used for the USING clause
    std::vector<lexeme_t> named_columns;

    if (parse_table_reference(ctx, cur_tok, left))
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
        out = std::make_unique<joined_table_t>(
                join_type, left, right, named_columns);
    else if (join_cond)
        out = std::make_unique<joined_table_t>(
                join_type, left, right, join_cond);
    else
        out = std::make_unique<joined_table_t>(join_type, left, right);
    return true;
}

bool parse_query_specification(
        parse_context_t& ctx,
        token_t& cur_tok,
        std::unique_ptr<query_specification_t>& out) {
    return false;
}

} // namespace sqltoast
