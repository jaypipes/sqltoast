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
    cur_sym = cur_tok.symbol;
    if (cur_sym != SYMBOL_RPAREN)
        goto err_expect_rparen;
    cur_tok = lex.next();
    goto optional_alias;
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
    goto push_table_reference;
err_expect_identifier:
    expect_error(ctx, SYMBOL_IDENTIFIER);
    return false;
push_table_reference:
    if (ctx.opts.disable_statement_construction)
        return true;
    if (table_name)
        out = std::make_unique<table_t>(table_name, alias);
    return true;
}

} // namespace sqltoast
