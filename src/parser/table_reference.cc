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
// <derived table> ::= <table subquery>
//
// <table subquery> ::= <subquery>
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
    join_type_t join_type = JOIN_TYPE_UNKNOWN;
    std::unique_ptr<search_condition_t> join_cond;
    std::unique_ptr<statement_t> derived;
    std::unique_ptr<table_reference_t> right;
    switch (cur_sym) {
        case SYMBOL_LPAREN:
            cur_tok = lex.next();
            goto expect_derived_table;
        case SYMBOL_IDENTIFIER:
            table_name = cur_tok.lexeme;
            cur_tok = lex.next();
            goto optional_alias;
        default:
            return false;
    }
expect_derived_table:
    if (! parse_select(ctx, cur_tok, derived))
        return false;
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
    goto ensure_derived_table;
err_expect_rparen:
    expect_error(ctx, SYMBOL_RPAREN);
    return false;
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
            goto process_cross_join;
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
        default:
            return true;
    }
process_cross_join:
    // We get here after successfully parsing a normal or derived table
    // followed by the CROSS symbol. We now expect a JOIN symbol followed by
    // another <table_reference>
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_JOIN)
        goto err_expect_join;
    cur_tok = lex.next();
    if (! parse_table_reference(ctx, cur_tok, right))
        goto err_expect_table_reference;
    goto push_cross_join;
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
ensure_normal_table:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<table_t>(table_name, alias);
    goto check_join;
ensure_derived_table:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<derived_table_t>(alias, derived);
    goto check_join;
push_cross_join:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<joined_table_t>(out, right);
    return true;
push_join:
    if (ctx.opts.disable_statement_construction)
        return true;
    out = std::make_unique<joined_table_t>(join_type, out, right, join_cond);
    return true;
}

} // namespace sqltoast
